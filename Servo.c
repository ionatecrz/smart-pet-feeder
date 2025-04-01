#include <xc.h>
#include <stdint.h>

#include "Retardo.h"
#include "Servo.h"

#define PIN_SERVO 9
#define CANTIDAD_AP 150

uint32_t t_alto = 1250;

void InicializarServo(void){
    
    ANSELA &= ~(1 << PIN_SERVO);
    
    SYSKEY = 0xAA996655;
    SYSKEY = 0x556699AA;
    RPB14R = 5; // OC2 a RPA9
    SYSKEY = 0x1CA11CA1;
    
    OC3CON = 0;
    OC3R = 1250;     // Tiempo en alto de 0.5 ms inicial
    OC3RS = 1250;
    OC3CON = 0x8006; // OC ON, modo PWM sin fallas
    
    T2CON = 0;
    TMR2 = 0;
    PR2 = 49999;     // Periodo de 20 ms
    T2CON = 0x8010;  // T2 ON, Div = 2
    
}

void sumaAngulo(uint32_t grados){ 
    
    t_alto += grados / 0.036; 

    if (t_alto > 6250){
        t_alto = 1250;
    } else if (t_alto < 1250){
        t_alto = 6250;
    }
    
    OC3RS = t_alto; 
}

uint32_t getGrados(void){
    return t_alto * 0.036 - 135;
}

uint32_t getTiempo(uint32_t cantidad){
    return 1000 * cantidad / 23; // 23 g/s
}

void dispensar(uint32_t cantidad){
    sumaAngulo(90);
    //Retardo(getTiempo(cantidad));
    Retardo(1000);
    sumaAngulo(-90);
}
