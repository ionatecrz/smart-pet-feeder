#ifndef SERVO_H
#define SERVO_H

#include <stdint.h>

void InicializarServo(void);
void sumaAngulo(uint32_t grados);
uint32_t getGrados(void);
void dispensar(uint32_t cantidad);
void apagarServo();
void encenderServo();

#endif