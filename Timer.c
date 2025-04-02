#include <xc.h>

static int ms = 0;
static int s = 0;
static int min = 0;
static int h = 0;

void InicializarTimer(void){
    T1CON = 0;
    TMR1 = 0;
    PR1 = 4999; 
    IPC1bits.T1IP = 2;  
    IPC1bits.T1IS = 0; 
    IFS0bits.T1IF = 0; 
    IEC0bits.T1IE = 1; 
    T1CON = 0x8000;

    INTCONbits.MVEC = 1; 
    asm("ei"); 
}

void __attribute__((vector(4), interrupt(IPL2SOFT), nomips16)) InterrupcionTimer1(void){
    IFS0bits.T1IF = 0;    

    ms++;
    if (ms == 1000){
        ms = 0;
        s++;
    }
    if (s == 60){
        s = 0;
        min++;
    }
    if (min == 60){
        min = 0;
        h++;
    }
    if (h == 24){
        h = 0;
    }
}

int getHoraActual(void){
    int copia;
    asm("di");
    copia = h;
    asm("ei");
    return copia;
}

int getMinutoActual(void){
    int copia;
    asm("di");
    copia = min;
    asm("ei");
    return copia;
}

int getSegundos(void) {
    int copia;
    asm("di");
    copia = s;
    asm("ei");
    return copia;
}

int getMilisegundos(void) {
    int copia;
    asm("di");
    copia = ms;
    asm("ei");
    return copia;
}
