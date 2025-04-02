#include <xc.h>
#include "Pic32Ini.h"
#include "Uart.h"
#include "Mascota.h"
#include "Timer.h"
#include "Buzzer.h"

#define PIN_PULSADOR 5
#define PIN_INPUT 4

int main(void) {
    
    TRISA = 0;
    TRISB = 1<<PIN_PULSADOR;
    TRISC = 1<<PIN_INPUT;
    
    LATA = 0;
    LATB = 0;
    LATC = 0xF;
    
    int hora1 = -1, min1 = -1;
    int hora2 = -1, min2 = -1;
    int peso = -1;
    char buffer[64];

    InicializarUART1(9600);
    InicializarTimer();
    InicializarBuzzer();
    
    int pulsador_ant = (PORTB>>PIN_PULSADOR) & 1;
    int pulsador_act;
    
    int estado_anterior = 0;
    int estado_confirmado = 0;
    int tiempo_cambio = -5;
    
    int minuto_anterior = -1;
    int rutina1_ejecutada = 0;
    int rutina2_ejecutada = 0;


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

        int minuto_actual = getMinutoActual();

        if (minuto_actual != minuto_anterior) {
            rutina1_ejecutada = 0;
            rutina2_ejecutada = 0;
            minuto_anterior = minuto_actual;
        }

        if (getHoraActual() == hora1 && minuto_actual == min1 && !rutina1_ejecutada) {
            putsUART("Hora de la primera comida!\n\r");
            dispensar(getRacion());
            reproducirMelodia();
            rutina1_ejecutada = 1;
        }

        if (getHoraActual() == hora2 && minuto_actual == min2 && !rutina2_ejecutada) {
            putsUART("Hora de la segunda comida!\n\r");
            dispensar(getRacion());
            reproducirMelodia();
            rutina2_ejecutada = 1;
        }

        
        pulsador_act = (PORTB>>PIN_PULSADOR) & 1;
        
        if (pulsador_act<pulsador_ant){
            clearUart();
        }
        
        int estado_actual = (PORTC >> PIN_INPUT) & 1;
        int tiempo_actual = getSegundos();
        
        if (estado_actual != estado_confirmado) {
            if (estado_actual != estado_anterior) {
                tiempo_cambio = tiempo_actual;
                estado_anterior = estado_actual;
            }

            if ((tiempo_actual - tiempo_cambio) >= 5) {
                estado_confirmado = estado_actual;

                if (estado_confirmado == 1) {
                    putsUART("Ha parado de comer!!!\n\r");
                } else {
                    putsUART("Esta comiendo!!!\n\r");
                }
            }
        }

        
        
        pulsador_ant = pulsador_act;
    }
}
