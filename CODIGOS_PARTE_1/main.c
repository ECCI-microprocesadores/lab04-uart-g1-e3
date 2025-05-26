#include <xc.h>
#include "uart.h"

#pragma config FOSC = INTIO67  // Oscilador interno
#pragma config WDTEN = OFF     // Watchdog Timer apagado
#pragma config LVP = OFF       // Low Voltage Programming off


void main(void) {
    OSCCON = 0b01110000;  // Oscilador interno a 16MHz
    UART_Init();          // Inicializa UART

    while(1) {
        UART_WriteString("Hola, UART funcionando!\r\n");
        __delay_ms(1000); 
    }
}