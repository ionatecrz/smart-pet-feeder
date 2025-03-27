#include <xc.h>
#include <stdint.h>

#include "Retardo.h"
#include "Servo.h"

#define PIN_SERVO 15
#define CANTIDAD_AP 150

uint32_t t_alto = 1250;

void InicializarServo(void){
    
    ANSELB &= ~(1 << PIN_SERVO);
    
    SYSKEY = 0xAA996655;
    SYSKEY = 0x556699AA;
    RPB15R = 5; // OC1 a RB15 
    SYSKEY = 0x1CA11CA1;
    
    OC1CON = 0;
    OC1R = 1250; // Tiempo en alto de 0,5 ms inicial
    OC1RS = 1250;
    OC1CON = 0x8006; // OC ON, modo PWM sin faltas
    
    T2CON = 0;
    TMR2 = 0;
    PR2 = 49999; // Periodo de 20 ms
    T2CON = 0x8010; // T2 ON, Div = 2
    
}

void sumaAngulo(uint32_t grados){ 
    
    t_alto += grados/0.072; 

    if (t_alto > 5000){
        t_alto = 2500;
    }else if (t_alto < 2500){
        t_alto = 5000;
    }
    
    OC1RS = t_alto; 
    
}

uint32_t getGrados(void){
    return t_alto*0.072-270;
}

uint32_t getTiempo(uint32_t cantidad){
    return 1000*cantidad/23; // 23 g/s
}

void dispensar(uint32_t cantidad){
    OC1RS = 5000;
    Retardo(getTiempo(cantidad));
    OC1RS = 2500;
    
}


