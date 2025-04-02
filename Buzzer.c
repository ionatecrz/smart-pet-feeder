#include <xc.h>
#include "Buzzer.h"

#define LONGITUD 36

static int ms = 0;
static int note = 0;
static int suena = 0;

static const int partitura[LONGITUD] = {
    MI, SOL, LA, SOL, MI, SILENCIO,
    MI, SOL, LA, SOL, MI, SILENCIO,
    SOL, LA, SI, DO_M, SI, LA, SOL, SILENCIO,
    MI, SOL, LA, SOL, MI, SILENCIO
};

static const int duracion[LONGITUD] = {
    400, 400, 400, 400, 400, 200,
    400, 400, 400, 400, 400, 200,
    400, 400, 400, 400, 400, 400, 800, 200,
    400, 400, 400, 400, 400, 1000
};

void InicializarBuzzer(void) {
    SYSKEY = 0xAA996655;
    SYSKEY = 0x556699AA;
    RPB3R = 5;
    SYSKEY = 0x1CA11CA1;

    T4CON = 0;
    TMR4 = 0;
    PR4 = 4999;
    IFS0bits.T4IF = 0;
    IEC0bits.T4IE = 1;
    IPC4bits.T4IP = 7;
    IPC4bits.T4IS = 3;
}

static void resetMilis(void) {
    asm("di");
    ms = 0;
    asm("ei");
}

static int GetMilis(void) {
    int copia_ms;
    asm("di");
    copia_ms = ms;
    asm("ei");
    return copia_ms;
}

static void setNota(int f_nota) {
    int pr3_c;

    T3CON = 0;
    OC1CON = 0;

    if (f_nota != SILENCIO) {
        pr3_c = 5000000 / f_nota - 1;
        OC1R = pr3_c / 2;
        OC1RS = pr3_c / 2;
        TMR3 = 0;
        PR3 = pr3_c;
        OC1CON = 0x800E;
        T3CON = 0x8000;
    }
}

void reproducirMelodia(void) {
    resetMilis();
    note = 0;
    setNota(partitura[note]);
    suena = 1;
    T4CON |= 0x8000;
}

void __attribute__((vector(16), interrupt(IPL7SOFT), nomips16)) InterrupcionTimer4(void) {
    IFS0bits.T4IF = 0;
    ms++;

    if (suena && GetMilis() >= duracion[note]) {
        resetMilis();
        note++;
        if (note >= LONGITUD) {
            T3CON = 0;
            OC1CON = 0;
            T4CON = 0;
            suena = 0;
        } else {
            setNota(partitura[note]);
        }
    }
}
