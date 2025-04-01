#include <xc.h>
#include <stdlib.h>
#include <string.h>
#include "Pic32Ini.h"
#include "Uart.h"
#include "Mascota.h"

#define TAM_COLA 200
#define PIN_U1RX 13
#define PIN_U1TX 7

#define MAX_MENSAJE 30

typedef struct {
    int icabeza;
    int icola;
    char datos[TAM_COLA];
} cola_t;

static cola_t cola_tx, cola_rx;
static char buffer[MAX_MENSAJE];
static int indice_buffer = 0;

static int nueva_hora1 = 0, nueva_hora2 = 0;
static int nueva_config_peso = 0;
static int peso_uart = -1;
static int hora1 = -1, min1 = -1, hora2 = -1, min2 = -1;

void InicializarUART1(int baudios) {
    ANSELB &= ~((1 << PIN_U1RX) | (1 << PIN_U1TX));
    TRISB |= (1 << PIN_U1RX);
    LATB |= (1 << PIN_U1TX);

    SYSKEY = 0xAA996655;
    SYSKEY = 0x556699AA;
    U1RXR = 3;
    RPB7R = 1;
    SYSKEY = 0x1CA11CA1;

    int bdrt;
    if (baudios <= 38400) {
        bdrt = 5000000 / (16 * baudios) - 1;
    } else {
        bdrt = 5000000 / (4 * baudios) - 1;
    }
    U1BRG = bdrt;

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

void enviarConfiguracionUART(void) {
    char mensaje[64];
    uint32_t peso_actual = getPeso();
    uint32_t racion_actual = getRacion()*2;

    sprintf(mensaje, "----- CONFIGURACION ACTUAL -----\n\r");
    putsUART(mensaje);

    sprintf(mensaje, "Peso configurado: %lu kg\n\r", peso_actual);
    putsUART(mensaje);

    sprintf(mensaje, "Racion diaria: %lu g\n\r", racion_actual);
    putsUART(mensaje);

    if (hora1 >= 0 && min1 >= 0) {
        sprintf(mensaje, "Primera comida: %02d:%02d\n\r", hora1, min1);
        putsUART(mensaje);
    } else {
        sprintf(mensaje, "Primera comida: No programada\n\r");
        putsUART(mensaje);
    }

    if (hora2 >= 0 && min2 >= 0) {
        sprintf(mensaje, "Segunda comida: %02d:%02d\n\r", hora2, min2);
        putsUART(mensaje);
    } else {
        sprintf(mensaje, "Segunda comida: No programada\n\r");
        putsUART(mensaje);
    }


    sprintf(mensaje, "--------------------------------\n\r");
    putsUART(mensaje);
}

void clearUart(void){
    putsUART("\033[2J\033[H");
}

void __attribute__((vector(32), interrupt(IPL3SOFT), nomips16)) InterrupcionUART1(void) {
    if (IFS1bits.U1RXIF == 1) {
        char c = U1RXREG;
        if (c == '\n' || c == '\r') {
            buffer[indice_buffer] = '\0';
            asm("di");
            if (strncmp(buffer, "Peso:", 5) == 0) {
                peso_uart = atoi(&buffer[5]);
                nueva_config_peso = 1;
            } else if (strncmp(buffer, "Primera Comida:", 15) == 0) {
                int t = atoi(&buffer[15]);
                hora1 = t / 100;
                min1 = t % 100;
                nueva_hora1 = 1;
            } else if (strncmp(buffer, "Segunda Comida:", 15) == 0) {
                int t = atoi(&buffer[15]);
                hora2 = t / 100;
                min2 = t % 100;
                nueva_hora2 = 1;
            }else if (strncmp(buffer, "Mostrar Config", 14) == 0) {
                enviarConfiguracionUART();
            }
            asm("ei");
            indice_buffer = 0;
        } else if (indice_buffer < MAX_MENSAJE - 1) {
            buffer[indice_buffer++] = c;
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

int hayNuevoPeso(void) {
    int res;
    asm("di");
    res = nueva_config_peso;
    nueva_config_peso = 0;
    asm("ei");
    return res;
}

int getPesoUART(void) {
    int p;
    asm("di");
    p = peso_uart;
    asm("ei");
    return p;
}

int hayPrimeraHoraNueva(void) {
    int res;
    asm("di");
    res = nueva_hora1;
    nueva_hora1 = 0;
    asm("ei");
    return res;
}

int getHoraPrimera(void) {
    int h;
    asm("di");
    h = hora1;
    asm("ei");
    return h;
}

int getMinPrimera(void) {
    int m;
    asm("di");
    m = min1;
    asm("ei");
    return m;
}

int haySegundaHoraNueva(void) {
    int res;
    asm("di");
    res = nueva_hora2;
    nueva_hora2 = 0;
    asm("ei");
    return res;
}

int getHoraSegunda(void) {
    int h;
    asm("di");
    h = hora2;
    asm("ei");
    return h;
}

int getMinSegunda(void) {
    int m;
    asm("di");
    m = min2;
    asm("ei");
    return m;
}
