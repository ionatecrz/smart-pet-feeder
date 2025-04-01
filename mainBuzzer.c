#include <xc.h>
#include "Pic32Ini.h"
#include "Buzzer.h"

#define PIN_PULSADOR 5

int main(void) {
    int pulsador_ant;
    int pulsador_act;

    InicializarBuzzer();

    INTCON |= 1 << 12;
    asm("ei");

    pulsador_ant = (PORTB >> PIN_PULSADOR) & 1;

    while (1) {
        pulsador_act = (PORTB >> PIN_PULSADOR) & 1;

        if ((pulsador_act != pulsador_ant) && (pulsador_act == 0)) {
            reproducirMelodia();
        }

        pulsador_ant = pulsador_act;
    }
}
