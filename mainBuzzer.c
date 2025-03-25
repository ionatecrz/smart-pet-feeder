#include <xc.h>
#include "Pic32Ini.h"
#include "Buzzer2.h"
#include <stdint.h>

#define PIN_PULSADOR 5
#define LONGITUD 48

int partitura[] = {
    DO, DO, SOL, SOL, LA, LA, SOL, SILENCIO,
    FA, FA, MI, MI, RE, RE, DO, SILENCIO,
    SOL, SOL, FA, FA, MI, MI, RE, SILENCIO,
    SOL, SOL, FA, FA, MI, MI, RE, SILENCIO,
    DO, DO, SOL, SOL, LA, LA, SOL, SILENCIO,
    FA, FA, MI, MI, RE, RE, DO, SILENCIO
};

int duracion[] = {
    400, 400, 400, 400, 400, 400, 800, 200,
    400, 400, 400, 400, 400, 400, 800, 200,
    400, 400, 400, 400, 400, 400, 800, 200,
    400, 400, 400, 400, 400, 400, 800, 200,
    400, 400, 400, 400, 400, 400, 800, 200,
    400, 400, 400, 400, 400, 400, 800, 1000
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
                T3CON = 0x8000;
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
