#ifndef UART_H
#define UART_H

void InicializarUART1(int baudios);
void putsUART(char s[]);
char getcUART(void);

int hayNuevoPeso(void);
int getPesoUART(void);

int hayPrimeraHoraNueva(void);
int getHoraPrimera(void);
int getMinPrimera(void);

int haySegundaHoraNueva(void);
int getHoraSegunda(void);
int getMinSegunda(void);

void enviarConfiguracionUART(void);
void clearUart(void);

void apagarUart();
void encenderUart();

#endif
