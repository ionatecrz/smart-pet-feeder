#include <xc.h>
#include <stdint.h>

#include "Pic32Ini.h"
#include "Servo.h"
#include "Mascota.h"
#include "Uart.h"
#include "Buzzer.h"


#define PIN_INPUT 4

int main(void) {
    
    ANSELC &= ~0xF & ~(1 << PIN_INPUT); 

    TRISA = 0;
    TRISB = 0;
    TRISC = 1<<PIN_INPUT;
   
    LATA = 0;
    LATB = 0;
    LATC = 0xF;

    
    while(1){
        
        if (PORTC>>PIN_INPUT & 1){
            LATCSET = 1;
        } else {
            LATCCLR = 1;
        }
        
    }

}

