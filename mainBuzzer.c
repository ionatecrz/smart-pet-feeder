#include <xc.h>
#include "Pic32Ini.h"
#include "Buzzer.h"
#include <stdint.h>

#define PIN_PULSADOR 5
#define LONGITUD 26

int partitura[] = {
    MI, SOL, LA, SOL, MI, SILENCIO,
    MI, SOL, LA, SOL, MI, SILENCIO,
    SOL, LA, SI, DO_M, SI, LA, SOL, SILENCIO,
    MI, SOL, LA, SOL, MI, SILENCIO
};

int duracion[] = {
    400, 400, 400, 400, 400, 200,
    400, 400, 400, 400, 400, 200,
    400, 400, 400, 400, 400, 400, 800, 200,
    400, 400, 400, 400, 400, 1000
};

int main(void)
{
    int pulsador_ant, pulsador_act, note = 0, suena = 0;

    initBuzzer();

    INTCON |= 1 << 12;
    asm("ei");

    pulsador_ant = (PORTB >> PIN_PULSADOR) & 1;

    while (1)
    {
        pulsador_act = (PORTB >> PIN_PULSADOR) & 1;

        if ((pulsador_act != pulsador_ant) && (pulsador_act == 0))
        {
            suena ^= 1;
            if (suena == 1)
            {
                resetMilis();
                note = 0;
                setNota(partitura[note]);
                T3CON |= 0x8000;
            }
            else
            {
                T4CON = 0;
                T3CON = 0;
                OC1CON = 0;
            }
        }

        pulsador_ant = pulsador_act;

        if (suena == 1)
        {
            if (GetMilis() >= duracion[note])
            {
                resetMilis();
                note++;
                if (note >= LONGITUD) note = 0;
                setNota(partitura[note]);
            }
        }
    }
}
