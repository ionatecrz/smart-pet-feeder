#include <xc.h>
#include <stdint.h>
#include "Retardo.h"
#include "Pic32Ini.h"

#define MAX_CONTADOR 65535
#define FREQ_TIMER 5000000

int Retardo(uint16_t retardo_ms){

    if (retardo_ms == 0){ 
        return 1;
    }

    uint8_t preesc;
    uint32_t contador = 0; 

    for (preesc = 0; preesc <= 3; preesc++){ 
        if (preesc!=3){
            contador = (retardo_ms * (FREQ_TIMER/1000) / (1 << preesc)) - 1;
        }else{
            contador = (retardo_ms * (FREQ_TIMER/1000) / (256)) - 1;
        }
   
        if (contador <= MAX_CONTADOR){
            break;
        }
    }

    if (contador > MAX_CONTADOR) {
        return 1;
    }

    T1CON = 0; 
    TMR1 = 0; 
    IFS0bits.T1IF = 0; 
    PR1 = contador; 
    T1CONbits.TCKPS = preesc;
    T1CONbits.ON = 1;

    while(IFS0bits.T1IF == 0); 
    IFS0bits.T1IF = 0;
    T1CON = 0; 
    
   
    return 0;
   }
   