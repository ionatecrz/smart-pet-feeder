#include <xc.h>
#include "Pic32Ini.h"

void inicializarTimer(void){

    //Configuracion del Timer 5
    T4CON = 0;
    T5CON = 0;
    T4CON = 8;
    TMR4 = 0;
    PR4 = 24999999; // Periodo orientativo: 5 segundos
    IPC2bits.T5IP = 2; //Prioridad = 2 
    IPC2bits.T5IS = 0; //Subprioridad = 0 
    IFS0bits.T5IF = 0; 
    IEC0bits.T5IE = 1; //Habilitamos la interrupcion 
    T4CON = 0x8008;

    INTCONbits.MVEC = 1; //Multivector
    asm("ei"); //Habilitamos globalmente las interrupciones
}

//Rutina de Interrupcion del Timer 5
void __attribute__((vector(20), interrupt(IPL2SOFT), nomips16)) InterrupcionTimer5(void){
    IFS0bits.T5IF = 0; //Limpiamos la bandera de interrupcion
}