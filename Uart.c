#include <xc.h>

#include "Pic32Ini.h"
#include "Uart.h"

#define TAM_COLA 100
#define PIN_U1RX 13
#define PIN_U1TX 7

typedef struct {
    int icabeza;
    int icola;
    char datos[TAM_COLA];
} cola_t;

static cola_t cola_tx, cola_rx;

void InicializarUART1(int baudios) {
    ANSELB &= ~((1 << PIN_U1RX) | (1 << PIN_U1TX));
    TRISB |= (1 << PIN_U1RX);
    LATB |= (1 << PIN_U1TX);

    SYSKEY = 0xAA996655;
    SYSKEY = 0x556699AA;
    U1RXR = 3;
    RPB7R = 1;
    SYSKEY = 0x1CA11CA1;

    int baudrate_bits;
    if (baudios <= 38400) {
        baudrate_bits = 5000000 / (16 * baudios) - 1;
    } else {
        baudrate_bits = 5000000 / (4 * baudios) - 1;
    }
    U1BRG = baudrate_bits;

    IFS1bits.U1RXIF = 0;
    IEC1bits.U1RXIE = 1;
    IFS1bits.U1TXIF = 0;
    IPC8bits.U1IP = 3;
    IPC8bits.U1IS = 1;
    
    U1STAbits.URXISEL = 0;
    U1STAbits.UTXISEL = 2;
    U1STAbits.URXEN = 1;
    U1STAbits.UTXEN = 1;
    U1MODE = 0x8000;
}

void putsUART(char s[]) {
    int indice = 0;
    asm("di");
    while (s[indice] != '\0') {
        if (((cola_tx.icabeza + 1) == cola_tx.icola) ||
            ((cola_tx.icabeza + 1) == TAM_COLA && cola_tx.icola == 0)) {
            break;
        } else {
            cola_tx.datos[cola_tx.icabeza] = s[indice];
            indice++;
            cola_tx.icabeza++;

            if (cola_tx.icabeza == TAM_COLA) {
                cola_tx.icabeza = 0;
            }
        }
    }
    asm("ei");
    IEC1bits.U1TXIE = 1;
}

char getcUART(void) {
    char c;
    asm("di");

    if (cola_rx.icola != cola_rx.icabeza) {
        c = cola_rx.datos[cola_rx.icola];
        cola_rx.icola++;
        if (cola_rx.icola == TAM_COLA) {
            cola_rx.icola = 0;
        }
    } else {
        c = '\0';
    }
    asm("ei");
    return c;
}

__attribute__((vector(32), interrupt(IPL3SOFT), nomips16)) void InterrupcionUART1(void) {
    if (IFS1bits.U1RXIF == 1) {
        if (!((cola_rx.icabeza + 1 == cola_rx.icola) ||
              (cola_rx.icabeza + 1 == TAM_COLA && cola_rx.icola == 0))) {
            cola_rx.datos[cola_rx.icabeza] = U1RXREG;
            cola_rx.icabeza++;
            if (cola_rx.icabeza == TAM_COLA) {
                cola_rx.icabeza = 0;
            }
        }
        IFS1bits.U1RXIF = 0;
    }

    if (IFS1bits.U1TXIF == 1) {
        if (cola_tx.icabeza != cola_tx.icola) {
            U1TXREG = cola_tx.datos[cola_tx.icola];
            cola_tx.icola++;
            if (cola_tx.icola == TAM_COLA) {
                cola_tx.icola = 0;
            }
        } else {
            IEC1bits.U1TXIE = 0;
        }
        IFS1bits.U1TXIF = 0;
    }
}
