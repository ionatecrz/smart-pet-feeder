#ifndef TIMERS_H
#define TIMERS_H

#include <stdint.h>

void InicializarTimers(void);
uint32_t getTiempoConfigurado(void);
void setTiempoConfigurado(uint32_t tiempo);

#endif
