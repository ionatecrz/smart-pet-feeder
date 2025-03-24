#include <xc.h>
#include <stdint.h>

#include "Pic32Ini.h"
#include "Servo.h"
#include "Mascota.h"

#define FREQ_TIMER 5000000
#define SEGUNDOS_12H 43200

static uint32_t ticks = 0;
static uint32_t temp_conf = 5;
static uint32_t int_12h = 0; // Nº de Interrupciones cada 12 horas

void InicializarTimers(void){
    T4CON = 0;
    T5CON = 0;
    T4CON = 8;
    TMR4 = 0;
    PR4 = (temp_conf * FREQ_TIMER) - 1;
    int_12h = SEGUNDOS_12H / temp_conf;
    IPC2bits.T5IP = 5;
    IPC2bits.T5IS = 0; 
    IFS0bits.T5IF = 0; 
    IEC0bits.T5IE = 1;
    T4CON = 0x8008;
}

uint32_t getTiempoConfigurado(void){
    uint32_t copy;
    asm("di");
    copy = temp_conf;
    asm("ei");
    return copy;
}

void setTiempoConfigurado(uint32_t tiempo){
    asm("di");
    uint32_t temp_ant = temp_conf;
    uint32_t int_12h_ant = int_12h;
    temp_conf = tiempo;
    PR4 = (temp_conf * FREQ_TIMER) - 1;
    int_12h = SEGUNDOS_12H / temp_conf;
    if (int_12h_ant != 0) {
        ticks = (ticks * int_12h) / int_12h_ant;
    } else {
        ticks = 0;
    }
    asm("ei"); 
}

void __attribute__((vector(20), interrupt(IPL5SOFT), nomips16)) InterrupcionTimer5(void) {
    IFS0bits.T5IF = 0;
    ticks++;

    if (ticks == int_12h || ticks == 2 * int_12h) { // Comidas Equiespaciadas
        dispensar(getRacion());
    }

    if (ticks >= 2 * int_12h) {
        ticks = 0;
    }
}