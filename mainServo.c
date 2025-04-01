#include <xc.h>
#include <stdint.h>

#include "Pic32Ini.h"
#include "Servo.h"

#define PIN_PULSADOR 5


int main(void) {

    TRISA = 0;
    TRISB = (1 << PIN_PULSADOR);
    TRISC = 0;
   
    LATA = 0;
    LATB = 0;
    LATC = 0xF;
    
    InicializarServo();

    int pulsador_ant = (PORTB >> PIN_PULSADOR) & 1;
    int pulsador_act;
    
    while(1){
        
        pulsador_act = (PORTB >> PIN_PULSADOR) & 1;
        
        if (pulsador_act < pulsador_ant){
            dispensar(0);
        }
        
        pulsador_ant = pulsador_act;
        
    }

    return 0;
}

