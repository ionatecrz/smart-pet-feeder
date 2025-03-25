#include <xc.h>
#include "Pic32Ini.h"
#include "Buzzer2.h"
#include <stdint.h>

#define PIN_PULSADOR 5
#define LONGITUD 32

int partitura[] = {
    MI, FA, SOL, LA, SOL, FA, MI, RE, 
    DO, RE, MI, FA, MI, RE, DO, SILENCIO,
    DO, RE, MI, FA, SOL, LA, SI, DO_M,
    DO_M, SI, LA, SOL, FA, MI, RE, DO
};

int duracion[] = {
    300, 300, 300, 300, 300, 300, 300, 300,
    300, 300, 300, 300, 300, 300, 600, 200,
    300, 300, 300, 300, 300, 300, 300, 600,
    300, 300, 300, 300, 300, 300, 300, 1000
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
                T2CON |= 0x8000;
            }
            else
            {
                T3CON = 0;
                T2CON = 0;
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
