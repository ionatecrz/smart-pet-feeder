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

#define PIN_INPUT 4

extern uint8_t SmallFont[];
extern const unsigned short dog[];

typedef enum {
    EST_BIENVENIDA,
    EST_INICIO,
    EST_DISPENSANDO,
    EST_ESTADO,
    EST_PERRITO  
} EstadoSistema;

void mostrarPerrito(void);
void mostrarInicio(void);
void mostrarEstado(int peso, int racion, int h1, int m1, int h2, int m2);
void animarDispensado(void);

char buffer_global[164];

void esperarMs(uint32_t ms) {
    uint32_t inicio = getTiempoAbsoluto();
    while ((getTiempoAbsoluto() - inicio) < ms);
}

int main(void) {
    TRISA = 0;
    TRISB = 1 << 5;
    TRISC = 1 << PIN_INPUT;

    LATA = 0;
    LATB = 0;
    LATC = 0xF;

    inicializarTFT(LANDSCAPE);
    setFont(SmallFont);
    InicializarUART1(9600);
    InicializarTimer();
    InicializarBuzzer();
    InicializarServo();
    clearUart();

    int peso = getPeso();
    int racion = getRacion();
    int hora1 = -1, min1 = -1;
    int hora2 = -1, min2 = -1;

    int estado_anterior = 0;
    int estado_confirmado = 0;
    int tiempo_cambio = -5;

    int minuto_anterior = -1;
    int rutina1_ejecutada = 0;
    int rutina2_ejecutada = 0;

    int mostrar_estado_comida = 0;

    EstadoSistema estado = EST_BIENVENIDA;
    EstadoSistema estado_anterior_sistema = -1;

    int uart_habilitada = 0;
    uint8_t sensor_habilitado = 0;
    uint32_t tiempo_inicio_sistema = getTiempoAbsoluto();
    uint32_t tiempo_inicio_estado_config = 0;
    uint32_t tiempo_inicio_bienvenida = getTiempoAbsoluto();
    uint8_t esperando_bienvenida = 1;

    mostrarInicio();

    while (1) {
        uint32_t ahora = getTiempoAbsoluto();

        if (esperando_bienvenida && ahora - tiempo_inicio_bienvenida >= 2000) {
            esperando_bienvenida = 0;
            uart_habilitada = 1;
            estado = EST_PERRITO;
        }

        if (estado == EST_ESTADO && ahora - tiempo_inicio_estado_config >= 4000) {
            estado = EST_PERRITO;
        }

        if (uart_habilitada && hayNuevoPeso()) {
            peso = getPesoUART();
            setPeso(peso);
            racion = getRacion();
            sprintf(buffer_global, "Peso actualizado: %d\n\r", peso);
            putsUART(buffer_global);

            mostrarEstado(peso, racion, hora1, min1, hora2, min2);
            estado = EST_ESTADO;
            tiempo_inicio_estado_config = ahora;
        }

        if (uart_habilitada && hayPrimeraHoraNueva()) {
            hora1 = getHoraPrimera();
            min1 = getMinPrimera();
            sprintf(buffer_global, "Primera comida: %02d:%02d\n\r", hora1, min1);
            putsUART(buffer_global);

            mostrarEstado(peso, racion, hora1, min1, hora2, min2);
            estado = EST_ESTADO;
            tiempo_inicio_estado_config = ahora;
        }

        if (uart_habilitada && haySegundaHoraNueva()) {
            hora2 = getHoraSegunda();
            min2 = getMinSegunda();
            sprintf(buffer_global, "Segunda comida: %02d:%02d\n\r", hora2, min2);
            putsUART(buffer_global);

            mostrarEstado(peso, racion, hora1, min1, hora2, min2);
            estado = EST_ESTADO;
            tiempo_inicio_estado_config = ahora;
        }

        if (!sensor_habilitado && (ahora - tiempo_inicio_sistema > 5000)) {
            sensor_habilitado = 1;
        }

        int minuto_actual = getMinutoActual();
        if (minuto_actual != minuto_anterior) {
            rutina1_ejecutada = 0;
            rutina2_ejecutada = 0;
            minuto_anterior = minuto_actual;
        }

        int hora_actual = getHoraActual();

        if (hora_actual == hora1 && minuto_actual == min1 && !rutina1_ejecutada) {
            reproducirMelodia();
            dispensar(getRacion());
            estado = EST_DISPENSANDO;
            rutina1_ejecutada = 1;
        }

        if (hora_actual == hora2 && minuto_actual == min2 && !rutina2_ejecutada) {
            reproducirMelodia();
            dispensar(getRacion());
            estado = EST_DISPENSANDO;
            rutina2_ejecutada = 1;
        }

        if (sensor_habilitado) {
            int lectura_estado = (PORTC >> PIN_INPUT) & 1;
            int tiempo_actual = ahora / 1000;

            if (lectura_estado != estado_confirmado) {
                if (lectura_estado != estado_anterior) {
                    tiempo_cambio = tiempo_actual;
                    estado_anterior = lectura_estado;
                }

                if ((tiempo_actual - tiempo_cambio) >= 5) {
                    estado_confirmado = lectura_estado;
                    if (estado_confirmado == 1) {
                        mostrar_estado_comida = 1;
                    }
                }
            }
        }

        if (estado != estado_anterior_sistema) {
            switch (estado) {
                case EST_INICIO:
                    mostrarInicio();
                    tiempo_inicio_bienvenida = ahora;
                    esperando_bienvenida = 1;
                    estado_anterior_sistema = estado;
                    break;

                case EST_ESTADO:
                    if (mostrar_estado_comida) {
                        mostrarEstado(peso, racion, hora1, min1, hora2, min2);
                        mostrar_estado_comida = 0;
                    }
                    estado_anterior_sistema = estado;
                    break;

                case EST_DISPENSANDO:
                    animarDispensado();
                    estado = EST_PERRITO;
                    estado_anterior_sistema = -1;
                    break;

                case EST_PERRITO:
                    mostrarPerrito();
                    estado_anterior_sistema = estado;
                    break;

                default:
                    break;
            }
        }
    }
}

void mostrarPerrito(void){
    clrScr();
    setColor(VGA_WHITE);
    print("Hola Perrito!", CENTER, 10, 0);
    drawBitmap(48, 30, 64, 64, dog, 1);
    setColor(VGA_RED);
    print("Es hora de comer!", CENTER, 100, 0);
}

void mostrarInicio(void) {
    clrScr();
    setColor(VGA_WHITE);
    print("Dispensador Canino", CENTER, 30, 0);
    print("Inteligente", CENTER, 50, 0);
    print("Inicializando sistema...", CENTER, 100, 0);
}

void mostrarEstado(int peso, int racion, int h1, int m1, int h2, int m2) {
    clrScr();
    setColor(VGA_RED);
    print("CONFIGURACION ACTUAL", CENTER, 5, 0);
    setColor(VGA_WHITE);

    sprintf(buffer_global, "Peso: %d kg", peso);
    print(buffer_global, LEFT, 30, 0);

    sprintf(buffer_global, "Racion: %d g", racion);
    print(buffer_global, LEFT, 50, 0);

    if (h1 >= 0 && m1 >= 0)
        sprintf(buffer_global, "1era comida: %02d:%02d", h1, m1);
    else
        sprintf(buffer_global, "1era comida: --:--");
    print(buffer_global, LEFT, 70, 0);

    if (h2 >= 0 && m2 >= 0)
        sprintf(buffer_global, "2da comida: %02d:%02d", h2, m2);
    else
        sprintf(buffer_global, "2da comida: --:--");
    print(buffer_global, LEFT, 90, 0);
}

void animarDispensado(void) {
    clrScr();
    setColor(VGA_RED);
    print("Dispensando comida!", CENTER, 30, 0);
    setColor(VGA_GREEN);
    for (int i = 0; i <= 100; i += 20) {
        fillRect(30, 70, 30 + i, 90);
        esperarMs(500);
    }
    setColor(VGA_WHITE);
    print("Listo! A comer", CENTER, 110, 0);
    esperarMs(1000);
}
