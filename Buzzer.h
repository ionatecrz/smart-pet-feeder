#ifndef BUZZER_H
#define BUZZER_H

#include <stdint.h>

void InicializarBuzzer();
void setAlarma(uint8_t intensidad);
void incrementarAlarma();
void encenderAlarma();
void apagarAlarma();

#endif