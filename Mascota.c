#include <xc.h>
#include <stdint.h>

static uint32_t peso = 10;

uint32_t getPeso(void){
    return peso;
}

void setPeso(uint32_t peso_kg){
    peso = peso_kg;
}

uint32_t getRacion(void) {

    uint32_t racion;

    if (peso <= 5) {
        racion = 25 + (uint32_t)((peso - 1) * (65.0 / 4.0)); // 25 a 90 g
    } else if (peso <= 10) {
        racion = 90 + (uint32_t)((peso - 5) * 12.0); // 90 a 150 g
    } else if (peso <= 20) {
        racion = 150 + (uint32_t)((peso - 10) * 15.0); // 150 a 300 g
    } else if (peso <= 30) {
        racion = 300 + (uint32_t)((peso - 20) * 10.0); // 300 a 400 g
    } else if (peso <= 40) {
        racion = 400 + (uint32_t)((peso - 30) * 10.0); // 400 a 500 g
    } else if (peso <= 50) {
        racion = 500 + (uint32_t)((peso - 40) * 10.0); // 500 a 600 g
    } else {
        racion = 600 + (uint32_t)((peso - 50) * 12.0); // estimación para > 50 kg
    }

    racion = (uint32_t) racion/2;

    return racion;
}

