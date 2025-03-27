#include <xc.h>
#include "Pic32Ini.h"
#include "Uart.h"
#include "Mascota.h"
#include "Timer.h"

int main(void) {
    int hora1 = -1, min1 = -1;
    int hora2 = -1, min2 = -1;
    int peso = -1;

    InicializarUART1(9600);
    InicializarTimer();

    while (1) {
        if (hayNuevoPeso()) {
            peso = getPesoUART();
            setPeso(peso);
            putsUART("Peso actualizado\n");
        }

        if (hayPrimeraHoraNueva()) {
            hora1 = getHoraPrimera();
            min1 = getMinPrimera();
            putsUART("Primera comida programada\n");
        }

        if (haySegundaHoraNueva()) {
            hora2 = getHoraSegunda();
            min2 = getMinSegunda();
            putsUART("Segunda comida programada\n");
        }

        if (getHoraActual() == hora1 && getMinutoActual() == min1) {
            putsUART("¡Hora de la primera comida!\n");
            // reproducirMelodia(); o acción equivalente
        }

        if (getHoraActual() == hora2 && getMinutoActual() == min2) {
            putsUART("¡Hora de la segunda comida!\n");
            // reproducirMelodia(); o acción equivalente
        }
    }
}
