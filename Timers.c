#include <xc.h>

#include "Pic32Ini.h"
#include "Servo.h"
#include "Mascota.h"

#define FREQ_TIMER 5000000
#define SEGUNDOS_12H 43200

static int ticks = 0;
static int temp_conf = 5;
static int int_12h = 0; // Nº de Interrupciones cada 12 horas

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
