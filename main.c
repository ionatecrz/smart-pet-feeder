#include <xc.h>
#include <stdint.h>

#include "Pic32Ini.h"
#include "Servo.h"
#include "Mascota.h"
#include "Uart.h"
#include "Buzzer.h"

#define PIN_PULSADOR 5

#define BAUDIOS 9600

int main(void) {
    
    ANSELB &= ~(1 << PIN_PULSADOR);
    ANSELC &= ~0xF; 

    TRISA = 0;
    TRISB = (1 << PIN_PULSADOR);
    TRISC = 0;
   
    LATA = 0;
    LATB = 0;
    LATC = 0xF;
    
    InicializarTimers();
    InicializarServo();
    InicializarBuzzer();
    InicializarUART1(BAUDIOS);

    INTCONbits.MVEC = 1; //Multivector
    asm("ei");

    int pulsador_ant = (PORTB >> PIN_PULSADOR) & 1;
    int pulsador_act;
    
    while(1){
        
        pulsador_act = (PORTB >> PIN_PULSADOR) & 1;
        
        if (pulsador_act < pulsador_ant){
            dispensar(getRacion());
        }
        
        pulsador_ant = pulsador_act;
        
    }

    return 0;
}

