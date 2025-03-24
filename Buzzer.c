#include <xc.h>
#include <stdint.h>

#include "Buzzer.h"

#define PIN_BUZZER 5

static uint8_t nivelAlarma = 0;
static uint8_t alarmaActivada = 1;

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

void incrementarAlarma() {
    nivelAlarma++;
    if (nivelAlarma > 3) {
        nivelAlarma = 3;
    }
    establecerNivel();
}

void encenderAlarma() {
    alarmaActivada = 1;
    establecerNivel();
}

void apagarAlarma() {
    alarmaActivada = 0;
    establecerNivel();
}


void establecerNivel(){
    if (alarmaActivada){
        OC1RS = nivelAlarma*1250;
    } else {
        OC1RS = 0;
    }
}

