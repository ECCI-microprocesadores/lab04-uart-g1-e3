#include <xc.h>
#include <stdio.h>
#include "ADC.h"
#include "UART.h"

#define _XTAL_FREQ 16000000

// Configuración del oscilador interno
#pragma config FOSC = INTIO67
#pragma config WDTEN = OFF
#pragma config LVP = OFF

void main(void) {
    OSCCON = 0x72; // Configura el oscilador interno a 16 MHz
    UART_Init();   // Inicializa UART
    ADC_Init();    // Inicializa ADC

    while (1) {
        unsigned int adc_value = ADC_Read(0); // Lee el canal AN0
        float voltage = (adc_value * 5.0) / 1023.0; // Conversión a voltaje
        char buffer[20];
        sprintf(buffer, "Voltaje: %.2fV\r\n", voltage);
        UART_WriteString(buffer);
        __delay_ms(1000);
    }
}
