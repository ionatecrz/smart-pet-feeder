#include <xc.h>
#include "Buzzer.h"

#define PIN_PULSADOR 5

static int ms = 0;

void initBuzzer(void)
{
    LATA = 0;
    LATB = 0;
    LATC = 0xF;

    TRISA = 0;
    TRISB = (1 << PIN_PULSADOR);
    TRISC = 0;

    SYSKEY = 0xAA996655;
    SYSKEY = 0x556699AA;
    RPB15R = 5; // OC1 → RA8
    SYSKEY = 0x1CA11CA1;

    T4CON = 0;
    TMR4 = 0;
    PR4 = 4999; // 1ms con Fpb = 5MHz
    IFS0bits.T4IF = 0;
    IEC0bits.T4IE = 1;
    IPC4bits.T4IP = 7;
    IPC4bits.T4IS = 3;
}

void __attribute__((vector(16), interrupt(IPL7SOFT), nomips16))InterrupcionTimer4(void)
{
    IFS0bits.T4IF = 0;
    ms++;
}

int GetMilis(void)
{
    int copia_ms;
    asm("di");
    copia_ms = ms;
    asm("ei");
    return copia_ms;
}

void resetMilis(void)
{
    asm("di");
    ms = 0;
    asm("ei");
}

void setNota(int f_nota)
{
    int pr3_c;

    T3CON = 0;
    OC1CON = 0;

    if (f_nota != SILENCIO)
    {
        pr3_c = 5000000 / f_nota - 1;
        OC1R = pr3_c / 2;
        OC1RS = pr3_c / 2;
        TMR3 = 0;
        PR3 = pr3_c;
        OC1CON = 0x800E;
        T3CON = 0x8000;
    }
}
