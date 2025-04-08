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
    EST_MENU,
    EST_DISPENSANDO,
    EST_ESTADO,
    EST_PERRITO  
} EstadoSistema;

void mostrarPerrito(void);
void mostrarInicio(void);
void mostrarMenu(void);
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

    putsUART("\n>>> Sistema INICIADO <<<\n\r");

    if (RCON & 0x01) putsUART("Reset por Power-on\n\r");
    if (RCON & 0x02) putsUART("Reset por Brown-out (bajo voltaje)\n\r");
    if (RCON & 0x04) putsUART("Reset por Watchdog Timer\n\r");
    if (RCON & 0x08) putsUART("Reset por fallo en Sleep\n\r");
    if (RCON & 0x10) putsUART("Reset por fallo en IDLE\n\r");
    if (RCON & 0x20) putsUART("Reset por MCLR (pin de reset externo)\n\r");
    if (RCON & 0x40) putsUART("Reset por Software (Soft reset)\n\r");
    RCON = 0;

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

    int mostrar_configuracion_temporal = 0;
    uint32_t tiempo_inicio_config = 0;

    esperarMs(2000);
    estado = EST_INICIO;

    while (1) {
        // ---------------- UART ----------------
        if (hayNuevoPeso()) {
            peso = getPesoUART();
            setPeso(peso);
            racion = getRacion();
            sprintf(buffer_global, "Peso actualizado: %d\n\r", peso);
            putsUART(buffer_global);

            mostrarEstado(peso, racion, hora1, min1, hora2, min2);
            mostrar_configuracion_temporal = 1;
            tiempo_inicio_config = getTiempoAbsoluto();
        }

        if (hayPrimeraHoraNueva()) {
            hora1 = getHoraPrimera();
            min1 = getMinPrimera();
            sprintf(buffer_global, "Primera comida: %02d:%02d\n\r", hora1, min1);
            putsUART(buffer_global);

            mostrarEstado(peso, racion, hora1, min1, hora2, min2);
            mostrar_configuracion_temporal = 1;
            tiempo_inicio_config = getTiempoAbsoluto();
        }

        if (haySegundaHoraNueva()) {
            hora2 = getHoraSegunda();
            min2 = getMinSegunda();
            sprintf(buffer_global, "Segunda comida: %02d:%02d\n\r", hora2, min2);
            putsUART(buffer_global);

            mostrarEstado(peso, racion, hora1, min1, hora2, min2);
            mostrar_configuracion_temporal = 1;
            tiempo_inicio_config = getTiempoAbsoluto();
        }

        if (mostrar_configuracion_temporal && (getTiempoAbsoluto() - tiempo_inicio_config >= 2000)) {
            mostrar_configuracion_temporal = 0;
            estado = EST_PERRITO;  
        }

 
        int minuto_actual = getMinutoActual();
        if (minuto_actual != minuto_anterior) {
            rutina1_ejecutada = 0;
            rutina2_ejecutada = 0;
            minuto_anterior = minuto_actual;
        }

        int hora_actual = getHoraActual();

        if (hora_actual == hora1 && minuto_actual == min1 && !rutina1_ejecutada) {
            esperarMs(500);
            reproducirMelodia();
            dispensar(getRacion());
            animarDispensado();
            mostrarEstado(peso, racion, hora1, min1, hora2, min2);
            rutina1_ejecutada = 1;
            estado = EST_DISPENSANDO;
        }

        if (hora_actual == hora2 && minuto_actual == min2 && !rutina2_ejecutada) {
            esperarMs(500);
            reproducirMelodia();
            dispensar(getRacion());
            animarDispensado();
            mostrarEstado(peso, racion, hora1, min1, hora2, min2);
            rutina2_ejecutada = 1;
            estado = EST_DISPENSANDO;
        }


        int lectura_estado = (PORTC >> PIN_INPUT) & 1;
        int tiempo_actual = getTiempoAbsoluto() / 1000;

        if (lectura_estado != estado_confirmado) {
            if (lectura_estado != estado_anterior) {
                tiempo_cambio = tiempo_actual;
                estado_anterior = lectura_estado;
            }

            if ((tiempo_actual - tiempo_cambio) >= 5) {
                estado_confirmado = lectura_estado;

                if (estado_confirmado == 1) {
                    putsUART("Ha parado de comer!!!\n\r");
                    mostrar_estado_comida = 1;
                    estado = EST_ESTADO;
                } else {
                    putsUART("Está comiendo!!!\n\r");
                }
            }
        }


        if (!mostrar_configuracion_temporal && estado != estado_anterior_sistema) {
            switch (estado) {
                case EST_INICIO:
                    mostrarInicio();
                    esperarMs(2000);
                    estado = EST_PERRITO;
                    break;
                case EST_MENU:
                    mostrarMenu();
                    break;
                case EST_DISPENSANDO:
                    esperarMs(1000);
                    estado = EST_PERRITO;
                    break;
                case EST_ESTADO:
                    if (mostrar_estado_comida) {
                        mostrarEstado(peso, racion, hora1, min1, hora2, min2);
                        mostrar_estado_comida = 0;
                        esperarMs(3000);
                        estado = EST_PERRITO;
                    }
                    break;
                case EST_PERRITO:
                    mostrarPerrito();
                    break;
                default:
                    break;
            }
            estado_anterior_sistema = estado;
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
    print("Inicilizando sistema...", CENTER, 100, 0);
}

void mostrarMenu(void) {
    clrScr();
    setColor(VGA_RED);
    print("MENU PRINCIPAL", CENTER, 10, 0);
    setColor(VGA_WHITE);
    print("Modo: Automatico", LEFT, 40, 0);
    print("Esperando señal...", LEFT, 60, 0);
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

void mostrarPerrito(void) {
    clrScr();
    setColor(VGA_WHITE);
    print("Hola Perrito!", CENTER, 10, 0);
    drawBitmap(48, 30, 64, 64, dog, 1);
    setColor(VGA_RED);
    print("Es hora de comer!", CENTER, 100, 0);
}

void animarDispensado(void) {
    clrScr();
    setColor(VGA_RED);
    print("Dispensando comida!", CENTER, 30, 0);
    setColor(VGA_GREEN);
    for (int i = 0; i <= 100; i += 20) {
        fillRect(30, 70, 30 + i, 90);
        esperarMs(300);
    }
    setColor(VGA_WHITE);
    print("Listo! A comer", CENTER, 110, 0);
}
