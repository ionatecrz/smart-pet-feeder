#include <xc.h>
#include <stdint.h>

#include "Buzzer.h"

#define PIN_BUZZER 5

static uint8_t nivelAlarma = 0;

void InicializarBuzzer(void){
    
    ANSELB &= ~(1 << PIN_BUZZER);
    
    SYSKEY = 0xAA996655;
    SYSKEY = 0x556699AA;
    RPB5R = 5; // OC2 a RB5 
    SYSKEY = 0x1CA11CA1;
    
    OC2CON = 0;
    OC2R = 0; 
    OC2RS = 0;
    OC2CON = 0x8006; // OC ON, modo PWM sin faltas
    
}

void setAlarma(uint8_t intensidad) {

    nivelAlarma = intensidad;

    if (!nivelAlarma) {
        OC1RS = 0;
    } else if (nivelAlarma==1) {
        OC1RS = 1250; // 25% duty cycle
    } else if (nivelAlarma==2) {
        OC1RS = 2500; // 50% duty cycle
    } else if (nivelAlarma==3) {
        OC1RS = 3750; // 75% duty cycle
    }
}