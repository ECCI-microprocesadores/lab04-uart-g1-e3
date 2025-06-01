#ifndef ADC_H
#define ADC_H

#include <xc.h>

// Prototipos de funciones
void ADC_Init(void);
unsigned int ADC_Read(unsigned char canal);

#endif // ADC_H
