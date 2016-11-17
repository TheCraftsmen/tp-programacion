#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/io.h>
#include <time.h>
#define P 0X70 /* RTC */


//Declaro funciones

unsigned char in (unsigned char reg);
void out (unsigned char valor, unsigned char reg);
void perm (unsigned long base, unsigned long cantidad, int permiso);
void leer_regA ();
int menu ();
char* pasar_a_binario (unsigned char num);
void mostrar_registros ();
void binario_o_bcd ();
void veinticuatrohoras_ampm();
void activar_alarma ();
void alarma_minuto ();
void tren_de_pulsos ();	
void puertoparalelo_hora();


// Empieza mi programa.

int main () {

    int opcion;
    perm ( P, 2, 1);

    do {
   
         opcion = menu ();
         switch (opcion){
  
              case 1:
                    mostrar_registros ();
                    break;

              case 2:
                    activar_alarma();
                    break;
           
              case 3:
                    alarma_minuto();
                    break;

              case 4:
                    tren_de_pulsos();
                    break;

              case 5:
                    puertoparalelo_hora();
                    break;
           
              case 6:
                    printf ( "\n Hasta luego!! \n");
                    break;

              default:
                    printf ("\n Opción inválida. \n");

      }


  } while (opcion != 6);

  perm ( P, 2, 0);
  return 0;

}



//Defino las funciones declaradas antes del main.



unsigned char in ( unsigned char reg) {

     outb (reg, P);
     return inb (P + 1);

}


void out (unsigned char valor, unsigned char reg){

     outb (reg, P);
     outb (valor, P + 1);

}



void perm (unsigned long base, unsigned long cantidad, int permiso){
     
     if (ioperm (base, cantidad, permiso)!=0){

                perror ("ioperm");
	        exit (1);
     }
}



void leer_regA(){

      unsigned char a;
      a= in(0x0a);
      if ((a&0x80)!=0);
         usleep (1984);

}



int menu (){

	int opcion;

	printf ("\n \n \n TP Programación 2016. Alumna: Julia Florencia Castro. Legajo n°5939. \n \n");
        printf ("\n Ingrese una opción válida y presione enter: \n");
	printf ("\n  1. Mostrar registros del RTC (7 AL D).");
	printf ("\n  2. Establecer alarma.");
        printf ("\n  3. Alarma (suena al segundo 00).");
        printf ("\n  4. Tren de pulsos de 4 Hz (Grafica 10).");
        printf ("\n  5. Imprimir hora del RTC por puerto paralelo.");
        printf ("\n  6. Salir del programa. \n");

	scanf ("%d", &opcion);
        return opcion;

}

char* pasar_a_binario (unsigned char num){
	int i, d, contador;
   	char *lista= NULL;
   	contador=0;
	lista=(char*)malloc(9);//pido memoria para 8 números + el /0
	for (i=8;i>0;i--)
   	{	d=num>>i;
		if (d&1)
         	{	*(lista+contador)=1+'0'; //si el resto es 1
      		}else
        	{	*(lista+contador)=0+'0'; //si resto es 0
		}
     		contador++;
   	}
	*(lista+contador) = '\0';

        return lista;
	
}


void mostrar_registros(){

        unsigned char dia, mes, an1, an2, regA, regB, regC, regD;
        leer_regA();

	dia=in(0x07);
        mes=in(0x08);
        an1=in(0x32);
        an2=in(0x09);
        regA=in(0x0a);
        regB=in(0X0b);
        regC=in(0x0c);
        regD=in(0x0d);

        printf("\n Descripcion     Dia          Mes         Año(--)      Año(--)"); 
        printf("\n Binario       %s     %s     %s     %s", pasar_a_binario(dia), pasar_a_binario(mes), pasar_a_binario(an1),
        pasar_a_binario(an2) ); 
        printf("\n Hexadecimal      %x          %x            %x          %x", dia, mes, an1, an2);
        printf("\n Decimal          %d          %d            %d          %d", dia, mes, an1, an2); 

        printf("\n \n Descripcion     Reg. A       Reg. B       Reg. C       Reg. D");
        printf("\n Binario        %s     %s     %s     %s", pasar_a_binario(regA), pasar_a_binario(regB), pasar_a_binario(regC), pasar_a_binario(regD) );
        printf("\n Hexadecimal      %x          %x            %x           %x", regA, regB, regC, regD);  
        printf("\n Decimal          %d          %d            %d          %d", regA, regB, regC, regD);

}



void binario_o_bcd(){

     unsigned char b;
     leer_regA;
     b=in(0x0b);
     if(b&0x04!=0)
          printf("\n El DM del regB se encuentra en modo binario");
          
     else
          printf("\n El DM del regB se encuentra en modo bcd");

}

void veinticuatrohoras_ampm(){
        unsigned char b;
        b=in(0x0b);
        if( (b&0x02)==0)
             printf ("\n El RTC esta en modo AM/PM.");
        else
             printf( "\n El RTC esta en modo 24hs.\n");
        return;         

}

void activar_alarma(){

	unsigned char hora, min, seg, b, c, i;
        binario_o_bcd();
        veinticuatrohoras_ampm();

        //printf("\n Ingrese la hora de alarma (hh:mm:ss):");
        //scanf("%hhx:%hhx:%hhx", &hora, &min, &seg);
 
        
	printf ("\nIngrese HH\n");
	scanf ("%hhx",&hora);
	printf ("Ingrese MM\n");
	scanf ("%hhx",&min);
	printf ("Ingrese SS\n");
	scanf ("%hhx",&seg);
        printf("\n Alarma ingresada correctamente");

        leer_regA();

        /*Configuro el Registro B*/
        
        b = in(0x0b);
        b = b & 0xdf; /* Desactivo el AIE */
        out(b, 0x0b);

        b = in(0x0b);
        b = b | 0x80; /* SET en 1 */
        out(b,0x0b);
 
        out(seg, 0x01); /*Setear segundos alarma*/
        out(min, 0x03); /*Setear minutos alarma*/
        out(hora, 0x05); /*Setear hora alarma*/

        b = b & 0x7f; /* Pongo el SET en 0 */
        out(b, 0x0b);
        
        b = b | 0x20; /* Activo el AIE (interruptor de la alarma) */
        out(b, 0x0b);

        c = in (0x0c); //borro flags anteriores
        

        for(i=0;i<180;i++){ /* hace 180 ciclos de 1 segundo y verifica el bit AF*/
            
            
            printf("\n Esperando alarma...");

 	    c = in(0x0c);

            if ((c & 0x20) != 0 ){
    	
                   printf ("\n ##### Alarma ##### \n");
                   printf ("\a \n"); /* beep */
                   return;
 
            }

            usleep(1000000); //esperar 1 segundo

        }
        
        printf("\n NO SE ACTIVO LA ALARMA");
        return;

}

void alarma_minuto(){

    unsigned char b, c, i;
    leer_regA();

    /* Configuro el Registro B */

     b = in(0x0b);
     b = b & 0xdf; /* Desactivo el AIE */
     out(b, 0x0b);

     b = in(0x0b);
     b = b | 0x80; //setear bit7 (SET)
     out(b,0x0b);

     out(0xff,0x03); /*Setear minutos alarma*/

     out(0xff,0x05); /*Setear hora alarma*/

     b = b & 0x7f; /* SET en 0 */
     out(b,0x0b);
     
     b = b | 0x20; /* AIE en 1 */
     out(b, 0x0b);

     printf("\n La alarma sonara cuando el segundero este en 00. \n Obs: Se activara 3 veces.\n");

     c = in(0x0c); //borro flags anteriores

     /*Hacer polling del reg C*/


     printf("\n Esperando las alarmas...\n");

     for(i=0;i<180;i++) {
          c = in(0x0c);
      
       //verificar bit 5 (AF)

         if(c & 0x20){
             printf("\n ###### Alarma ###### \n");
             printf("\a \n"); // beep
         }
         usleep (1000000);
         
      }
      return;

}


void tren_de_pulsos (){

  unsigned char a, b, c, aux, aux2, i;
  
  leer_regA();
  aux=in(0x0a);
  a=aux|0x0e; //seteo los bits del registro A para activar la frecuencia 4Hz.
  out(a,0x0a);

  aux2=in(0x0b);
  b=aux2|0x48;
  out(b,0x0b);//seteo los bits PIE y SQWE para activar la interrupción periódica.


  c=in(0x0c); //borro flags anteriores

  /*Hacer polling del registro C*/
  
  printf("\n0---+---1---+---2---+---3---+---4---+---5---+---6---+---7---+---8---+---9---+---\n");
  
  for (i=0; i<80; i++){ // 80 ciclos porque son 8 caracteres por cada segundo
      
       c=in(0x0c);
       if((c&0x40)==0) // Me fijo si se activa el PF
           printf("_");
       else
           printf("|");
       fflush(stdout);
       usleep(125000); //esperar 1/8 segundos
      
  }

return;

}



void puertoparalelo_hora(){

  unsigned char hora;
  leer_regA();
  hora=in(0x04);
  //printf("\n Hora del RTC: %hhx", hora);
  //printf("\n Hora por puerto paralelo: %hhx", ~hora);

  perm(0x378, 3, 1);

  hora = ~hora;
  outb(hora, 0x378);
  outb(0, 0x37a); //strobe en 0
  usleep(100); //1 microsegundo
  outb(0xff, 0x37a); //strobe en 1

  perm(0x378, 3, 0);

  return;
  
}




