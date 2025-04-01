#include <xc.h>
#include "Pic32Ini.h"
#include "Uart.h"
#include "Mascota.h"
#include "Timer.h"

#define PIN_PULSADOR 5

int main(void) {
    
    TRISA = 0;
    TRISB = 1<<PIN_PULSADOR;
    TRISC = 0;
    
    LATA = 0;
    LATB = 0;
    LATC = 0;
    
    int hora1 = -1, min1 = -1;
    int hora2 = -1, min2 = -1;
    int peso = -1;
    char buffer[64];

    InicializarUART1(9600);
    InicializarTimer();
    
    int pulsador_ant = (PORTB>>PIN_PULSADOR) & 1;
    int pulsador_act;

    while (1) {
        if (hayNuevoPeso()) {
            peso = getPesoUART();
            setPeso(peso);
            sprintf(buffer, "Peso actualizado: %d\n\r", peso);
            putsUART(buffer);
        }

        if (hayPrimeraHoraNueva()) {
            hora1 = getHoraPrimera();
            min1 = getMinPrimera();
            sprintf(buffer, "Primera comida programada a las %02d:%02d\n\r", hora1, min1);
            putsUART(buffer);
        }

        if (haySegundaHoraNueva()) {
            hora2 = getHoraSegunda();
            min2 = getMinSegunda();
            sprintf(buffer, "Segunda comida programada a las %02d:%02d\n\r", hora2, min2);
            putsUART(buffer);
        }

        if (getHoraActual() == hora1 && getMinutoActual() == min1) {
            putsUART("Hora de la primera comida!\n\r");
            reproducirMelodia();
            putsUART("Hola\n\r");
        }

        if (getHoraActual() == hora2 && getMinutoActual() == min2) {
            putsUART("Hora de la segunda comida!\n\r");
        }
        
        pulsador_act = (PORTB>>PIN_PULSADOR) & 1;
        
        if (pulsador_act<pulsador_ant){
            clearUart();
        }
        
        pulsador_ant = pulsador_act;
    }
}
