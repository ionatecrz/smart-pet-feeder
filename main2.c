#include <xc.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "Pic32Ini.h"
#include "TftDriver/TftDriver.h"
#include "Uart.h"
#include "Mascota.h"
#include "Timer.h"
#include "Buzzer.h"
#include "Servo.h"

#define PIN_PULSADOR 5
#define PIN_INPUT 4

extern uint8_t SmallFont[];
extern const unsigned short dog[];

// Prototipos TFT
void pause(void);
void mostrarPerrito(void);
void mostrarInicio(void);
void mostrarMenu(void);
void mostrarEstado(int peso);
void animarDispensado(void);
void mostrarAlerta(void);

int main(void) {
    // Configuración puertos
    TRISA = 0;
    TRISB = 1<<PIN_PULSADOR;
    TRISC = 1<<PIN_INPUT;

    LATA = 0;
    LATB = 0;
    LATC = 0xF;

    // Inicializaciones
    inicializarTFT(LANDSCAPE);
    setFont(SmallFont);
    InicializarUART1(9600);
    InicializarTimer();
    InicializarBuzzer();
    InicializarServo();

    // Variables de control
    int hora1 = -1, min1 = -1;
    int hora2 = -1, min2 = -1;
    int peso = -1;
    char buffer[64];

    int pulsador_ant = (PORTB>>PIN_PULSADOR) & 1;
    int pulsador_act;

    int estado_anterior = 0;
    int estado_confirmado = 0;
    int tiempo_cambio = -5;

    int minuto_anterior = -1;
    int rutina1_ejecutada = 0;
    int rutina2_ejecutada = 0;

    clearUart();

    mostrarPerrito(); pause();
    mostrarInicio(); pause();
    mostrarMenu(); pause();

    while (1) {
        // Lectura de peso
        if (hayNuevoPeso()) {
            peso = getPesoUART();
            setPeso(peso);
            sprintf(buffer, "Peso actualizado: %d\n\r", peso);
            putsUART(buffer);
        }

        // Programación de horarios
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
            reproducirMelodia();
            animarDispensado();
            rutina1_ejecutada = 1;
        }

        if (getHoraActual() == hora2 && minuto_actual == min2 && !rutina2_ejecutada) {
            putsUART("Hora de la segunda comida!\n\r");
            reproducirMelodia();
            animarDispensado();
            rutina2_ejecutada = 1;
        }

        // Interacción con el pulsador
        pulsador_act = (PORTB>>PIN_PULSADOR) & 1;

        if (pulsador_act < pulsador_ant) {
            reproducirMelodia();
            animarDispensado();
            mostrarEstado(peso);
            pause();
        }

        // Lectura del estado del perro
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

// --- FUNCIONES TFT ---
void pause(void)
{
    while(PORTB & (1<<PIN_PULSADOR));
    while(!(PORTB & (1<<PIN_PULSADOR)));
}

void mostrarPerrito(void)
{
    clrScr();
    setColor(VGA_WHITE);
    print("Hola Perrito!", CENTER, 10, 0);
    drawBitmap(48, 30, 64, 64, dog, 1);
    setColor(VGA_RED);
    print("Es hora de comer!", CENTER, 100, 0);
}

void mostrarInicio(void)
{
    clrScr();
    setColor(VGA_WHITE);
    print("Dispensador Canino", CENTER, 30, 0);
    print("Inteligente", CENTER, 50, 0);
    print("Presiona RB5 para continuar", CENTER, 100, 0);
}

void mostrarMenu(void)
{
    clrScr();
    setColor(VGA_RED);
    print("MENU PRINCIPAL", CENTER, 10, 0);
    setColor(VGA_WHITE);
    print("1. Dispensar ahora", LEFT, 40, 0);
    print("2. Modo automatico", LEFT, 60, 0);
    print("3. Ver estado", LEFT, 80, 0);
}

void mostrarEstado(int peso)
{
    clrScr();
    setColor(VGA_RED);
    print("Estado del sistema", CENTER, 10, 0);
    char buffer[32];
    setColor(VGA_WHITE);
    sprintf(buffer, "Comida restante: %d g", peso);
    print(buffer, LEFT, 40, 0);
    print("Ultima vez: --:--", LEFT, 60, 0); // Puedes completar con RTC
    print("Modo: Automatico", LEFT, 80, 0);
}

void animarDispensado(void)
{
    clrScr();
    setColor(VGA_RED);
    print("Dispensando comida!", CENTER, 30, 0);
    setColor(VGA_GREEN);
    for (int i = 0; i <= 100; i += 20)
    {
        fillRect(30, 70, 30 + i, 90);
        // Delay de tiempo si se desea
    }
    setColor(VGA_WHITE);
    print("Listo! A comer", CENTER, 110, 0);
}

void mostrarAlerta(void)
{
    fillScr(VGA_RED);
    setColor(VGA_WHITE);
    print("Atencion!", CENTER, 30, 0);
    print("Nivel de comida bajo", CENTER, 60, 0);
    print("Por favor recarga el tanque", CENTER, 80, 0);
}
