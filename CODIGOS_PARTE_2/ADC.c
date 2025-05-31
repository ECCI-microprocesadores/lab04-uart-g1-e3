#include <xc.h>
#include "ADC.h"

#define _XTAL_FREQ 16000000

void ADC_Init(void) {
    TRISAbits.TRISA0 = 1;       // AN0 como entrada
    ANSELAbits.ANSA0 = 1;       // AN0 como analógico

    ADCON0 = 0x01;              // Canal AN0, ADC ON
    ADCON1 = 0x00;              // Justificación izquierda
    ADCON2 = 0xA9;              // Tacq 8TAD, Fosc/8
}

unsigned int ADC_Read(unsigned char canal) {
    ADCON0 &= 0xC5;             // Limpia bits de selección de canal
    ADCON0 |= (canal << 2);     // Selecciona nuevo canal
    __delay_ms(2);              // Tiempo de adquisición
    ADCON0bits.GO = 1;          // Inicia conversión
    while (ADCON0bits.GO);      // Espera a que termine

    return ((unsigned int)(ADRESH << 8) | ADRESL);
}
