[![Open in Visual Studio Code](https://classroom.github.com/assets/open-in-vscode-2e0aaae1b6195c2367325f4f02e2d04e9abb55f0b24a779b69b11b9e10269abc.svg)](https://classroom.github.com/online_ide?assignment_repo_id=19504450&assignment_repo_type=AssignmentRepo)
# Lab04: Comunicación UART en PIC18F45K22

## Integrantes

[JHON ALEXANDER CUADROS LAIS](https://github.com/JhonCuadros)

[MIGUEL ANGEL CUERVO](https://github.com/MiguelAcuervo)

# INTRODUCCIÓN
 
 La comunicación UART (Universal Asynchronous Receiver Transmitter) es un protocolo ampliamente utilizado en sistemas embebidos para la transmisión de datos serie de forma asincrónica entre dispositivos. El microcontrolador **PIC18F45K22** integra un módulo **EUSART** que permite implementar esta comunicación de manera eficiente y configurable, facilitando la interacción con distintos periféricos y herramientas externas.

Esta práctica de laboratorio se divide en dos partes:

- En la **Parte 1**, se establece una comunicación básica entre el **PIC18F45K22** y una computadora mediante UART, con el objetivo de enviar un mensaje desde el microcontrolador y visualizarlo en una terminal serial utilizando el programa **PuTTY**. Esta etapa permite familiarizarse con la configuración del módulo EUSART y el envío de datos en formato de texto.

- En la **Parte 2**, se amplía la funcionalidad del sistema al integrar la comunicación UART con un script en **Python**, que recibe datos enviados por el microcontrolador relacionados con el valor de un potenciómetro leído a través del conversor analógico-digital (**ADC**). El script procesa estos datos, extrae el voltaje correspondiente y genera una visualización gráfica en tiempo real utilizando la librería **Matplotlib**. Esta sección permite comprender cómo establecer una interfaz entre el microcontrolador y una aplicación de software, promoviendo el análisis y visualización de datos adquiridos por sensores.

En conjunto, la práctica permite reforzar los conceptos de comunicación serial, configuración del EUSART en el PIC, lectura de señales analógicas mediante el ADC y la integración con herramientas externas para el procesamiento de datos, lo cual es esencial en el desarrollo de sistemas embebidos interactivos.

# OBJETIVOS

- Configurar el módulo EUSART del PIC18F45K22 para transmitir datos de forma serial a través del protocolo UART.
- Visualizar mensajes enviados desde el microcontrolador en una terminal serial mediante el software **PuTTY**.
- Leer el valor analógico de un potenciómetro utilizando el módulo ADC del PIC18F45K22.
- Enviar los valores leídos del ADC por UART hacia una computadora para su posterior análisis.
- Implementar un script en **Python** capaz de recibir datos UART y representar gráficamente el voltaje leído en tiempo real utilizando **Matplotlib**.

# Conceptos Fundamentales

Para implementar correctamente la comunicación UART y la lectura del valor de un potenciómetro mediante el ADC en el microcontrolador PIC18F45K22, es fundamental comprender el uso y configuración de ciertos registros internos. A continuación, se describen los registros más relevantes:

### Comunicación UART - Módulo EUSART

El módulo **EUSART (Enhanced Universal Synchronous Asynchronous Receiver Transmitter)** permite la transmisión y recepción de datos de manera serie, asincrónica o sincrónica. Para su configuración en modo asincrónico (UART), se deben tener en cuenta los siguientes registros:

- **TXSTA (Transmit Status and Control Register):**
  - `TXEN`: Habilita la transmisión.
  - `BRGH`: Selecciona alta o baja velocidad para el generador de baudios.
  - `SYNC`: Debe estar en 0 para habilitar el modo asincrónico.

- **RCSTA (Receive Status and Control Register):**
  - `SPEN`: Habilita el puerto serial (RX/TX).
  - `CREN`: Habilita la recepción continua.
  - `OERR` y `FERR`: Indicadores de errores de recepción.

- **BAUDCON (Baud Rate Control Register):**
  - `BRG16`: Selecciona el uso de un generador de 8 o 16 bits para el baud rate.

- **SPBRG y SPBRGH (Baud Rate Generator Registers):**
  - Establecen el valor para configurar la velocidad de transmisión según el valor del reloj del sistema (Fosc) y el modo BRGH/BRG16.

  - Fórmula usada para calcular el baudrate (cuando BRGH = 0 y BRG16 = 0):
  $$SPBRG=\frac{f_{osc}}{64×Baudrate}−1$$

- **TXREG y RCREG:**
  - `TXREG`: Registro donde se escribe el dato a transmitir.
  - `RCREG`: Registro donde se almacena el dato recibido.

### Conversión Analógica a Digital (ADC)

Para adquirir el valor de un potenciómetro, se utiliza el módulo **ADC** del PIC18F45K22, que convierte una señal analógica (0–5 V) en un valor digital. Los registros involucrados son:

- **ADCON0 (A/D Control Register 0):**
  - `ADON`: Habilita el módulo ADC.
  - `GO/DONE`: Inicia la conversión y permite verificar cuándo ha finalizado.
  - Selección del canal analógico (bits `CHS<3:0>`).

- **ADCON1 (A/D Control Register 1):**
  - Configura el tipo de justificación (izquierda o derecha) y las referencias de voltaje.

- **ADCON2 (A/D Control Register 2):**
  - Determina el tiempo de adquisición y el tiempo del convertidor.
  - `ADFM`: Define la alineación del resultado (0 = izquierda, 1 = derecha).

- **ADRESH y ADRESL:**
  - Registros donde se almacena el resultado de la conversión.
  - Si la justificación es a la derecha, se debe combinar `ADRESH` y `ADRESL` para obtener el valor completo (10 bits).

El conocimiento y la correcta configuración de estos registros son esenciales para el éxito de la práctica, ya que permiten controlar la comunicación UART y la adquisición precisa del valor analógico del potenciómetro.

# DESCRIPCIÓN 

## Descripción del Código PARTE 1 

## Archivo Principal `main.c`

#### **Configuraciones por directivas `#pragma config`**

```c 
#include <xc.h>
#include "uart.h"

#pragma config FOSC = INTIO67  // Oscilador interno
#pragma config WDTEN = OFF     // Watchdog Timer apagado
#pragma config LVP = OFF       // Low Voltage Programming off
```

- `FOSC = INTIO67:` Usa el oscilador interno con los pines RA6 y RA7 disponibles como I/O.

- `WDTEN = OFF:` Desactiva el Watchdog Timer.

- `LVP = OFF:` Desactiva la programación en bajo voltaje.

### **Configuración del reloj**

```c 
void main(void) {
    OSCCON = 0b01110000;  // Oscilador interno a 16MHz
    UART_Init();          // Inicializa UAR
```

**Registro OSCCON (Oscillator Control Register)**
```c 
    OSCCON = 0b01110000;  // Oscilador interno a 16MHz
```
La instrucción configura el registro OSCCON (Oscillator Control Register), el cual controla el tipo de oscilador que se usa y su frecuencia cuando se utiliza el oscilador interno del microcontrolador PIC18F45K22.

**Desglose del valor `0b01110000`**

Este valor es binario y representa los bits individuales del registro OSCCON, que tiene esta estructura:

| Bit | Nombre | Valor | Descripción                                                                 |
|-----|--------|--------|------------------------------------------------------------------------------|
| 7   | IDLEN  | 0      | Entra en modo Sleep (no Idle) cuando se ejecuta `SLEEP`.                     |
| 6   | IRCF2  | 1      | Bit alto de la selección de frecuencia.                                      |
| 5   | IRCF1  | 1      | Bit medio de la selección de frecuencia.                                     |
| 4   | IRCF0  | 1      | Bit bajo de la selección de frecuencia.                                      |
| 3   | OSTS   | 0      | Solo lectura. Indica que no se está usando el oscilador primario.            |
| 2   | HFIOFS | 0      | Solo lectura. Indica si el oscilador de alta frecuencia está estable.        |
| 1   | SCS1   | 0      | Bit de selección de la fuente de reloj.                                      |
| 0   | SCS0   | 0      | Bit de selección de la fuente de reloj.     

**¿Qué es `IRCF<2:0>`?**

Corresponde a los bits 6-4 del registro `OSCCON` y determinan la **frecuencia de salida del oscilador interno**:

| IRCF2 | IRCF1 | IRCF0 | Frecuencia |
|--------|--------|--------|-------------|
| 1      | 1      | 1      | 16 MHz      |
| 1      | 1      | 0      | 8 MHz       |
| 1      | 0      | 1      | 4 MHz       |
| 1      | 0      | 0      | 2 MHz       |
| 0      | 1      | 1      | 1 MHz       |
| ...    | ...    | ...    | ...         |

En este caso, IRCF = 111 configura el oscilador interno a 16 MHz, lo cual es fundamental para garantizar que los retardos y la comunicación UART funcionen correctamente.

**Bucle principal**

```c 
  while(1) {
        UART_WriteString("Hola, UART funcionando!\r\n");
        __delay_ms(1000);        // Inicializa UAR
```

**`while(1)`**

- Se trata de un bucle infinito.

- El valor 1 representa verdadero, por lo tanto la condición siempre se cumple, y el contenido del bucle se ejecutará repetidamente mientras el sistema esté encendido.

**`UART_WriteString("Hola, UART funcionando!\r\n");`**

- Esta función (definida en uart.c) envía una cadena de caracteres por el puerto serial UART.

- En este caso, se transmite el texto "Hola, UART funcionando!", seguido de \r\n, que corresponde a:

  - `\r: retorno de carro (carriage return)`.

  - `\n: nueva línea (line feed)`.

- Estos caracteres aseguran que el texto se muestre correctamente en el monitor serial (como PuTTY), comenzando en una nueva línea tras cada mensaje.

**`__delay_ms(1000);`**

- Esta es una función proporcionada por el compilador XC8 (cuando se define _XTAL_FREQ correctamente).

- Provoca un retardo de 1000 milisegundos, es decir, 1 segundo.

- Permite espaciar los mensajes enviados para que no se envíen de forma continua y saturen la UART o el monitor serial.

## **Archivo `uart.c` – Implementación de la comunicación UART**

### **Encabezados**

```c 
#include "uart.h"
#include <stdio.h>
```

- `uart.h`: Cabecera propia, donde se definen las funciones UART (Init, WriteChar, WriteString) y se declara la frecuencia del reloj.

- `<stdio.h>`: Incluido por compatibilidad (por ejemplo, con futuras funciones como sprintf), pero no se utiliza directamente en este código.

**`UART_Init(void)`**

Esta función configura el módulo EUSART (Enhanced Universal Synchronous Asynchronous Receiver Transmitter) del PIC para comunicación asíncrona a 9600 baudios con Fosc = 16 MHz.

**Configuración de pines TX y RX**

```c 
TRISC6 = 0; // TX (RC6) como salida
TRISC7 = 1; // RX (RC7) como entrada
```

- `RC6` es la salida del transmisor UART (TX).

- `RC7` es la entrada del receptor UART (RX).

- `TRISx`: Registros de dirección de los puertos:

  - `0` = salida

  - `1` = entrada

**¿Qué son TX y RX?**

**`TX (Transmit):`**

- Es el pin por donde el microcontrolador envía datos hacia otro dispositivo.
En el PIC18F45K22, este pin es el RC6 (puerto C, pin 6), también conocido como TX1 cuando se usa el módulo EUSART1.

**`RX (Receive):`**

- Es el pin por donde el microcontrolador recibe datos desde otro dispositivo.
En este micro, es el RC7, también llamado RX1.

Estos dos pines se utilizan para la comunicación UART (asíncrona, serie) con otros dispositivos como:

 - PCs (mediante un adaptador USB-UART),

 - otros microcontroladores,

 - módulos Bluetooth,

 - módulos GSM, GPS, etc.

### **Configuración de Baudrate (velocidad de transmisión)**

 ```c 
SPBRG1 = 25; // Para 9600 baudios con Fosc = 16MHz
TXSTA1bits.BRGH = 0; // BRGH = 0 → baja velocidad
BAUDCON1bits.BRG16 = 0; // BRG16 = 0 → generador de 8 bits
```

#### **Cálculo del baudrate (cuando BRGH = 0 y BRG16 = 0):**
$$
\
BaudRate = \frac{F_{osc}}{64 \times (SPBRG1 + 1)} = \frac{16\,MHz}{64 \times (25 + 1)} = 9600
\
$$

- `SPBRG1`: Registro base del baudrate (solo 8 bits al tener `BRG16 = 0`)

- `TXSTA bits.BRGH`:

  - Es un bit de configuración del registro TXSTA1 (Transmit Status and Control Register) que determina la velocidad del baudrate (tasa de transmisión de bits) cuando se usa el módulo UART en modo asíncrono.

    - 0 = baja velocidad (usa divisor de 64)
    - 1 = alta velocidad (usa divisor de 16)

- `BAUDCON1 bits.BRG16`:
  - 0 = usa solo `SPBRG1` (8 bits)
  - 1 = habilita `SPBRGH1` también (16 bits en total)

#### **¿Qué es el Baudrate?**

- Es la velocidad a la que se transmiten los bits por segundo (bps).
Por ejemplo, un baudrate de 9600 significa que se transmiten 9600 bits por segundo.

- El baudrate debe coincidir exactamente en ambos dispositivos que se comunican (ejemplo: microcontrolador ↔ PC).

#### **¿Qué hace internamente?**

- Cuando se configura BRGH = 0, el hardware del PIC usa un divisor más grande (÷64), lo que resulta en una velocidad de transmisión más baja o menos precisa a frecuencias bajas.

- Cuando BRGH = 1, el divisor se reduce (÷16), lo que aumenta el baudrate y mejora la precisión del cálculo del baudrate deseado. También permite alcanzar baudrates más altos.

####  **Fórmula general para calcular `SPBRG1`**

El valor de `SPBRG1` se calcula con esta fórmula (cuando `BRGH = 0` y `BRG16 = 0`):

$$
\
Baudrate = \frac{F_{osc}}{64 \times (SPBRG + 1)}
\
$$
Donde:

- $F_{osc}$ es la frecuencia del reloj del microcontrolador (en este caso 16 MHz)

- `SPBRG` es el valor que se cargo en el registro para ajustar el baudrate


#### **Cálculo para este caso**

Queremos:

- $F_{osc} = 16\,MHz$
- Baudrate = 9600 bps

Sustituimos en la fórmula:

$$
\
9600 = \frac{16 \times 10^6}{64 \times (SPBRG + 1)}
\
$$

Despejamos `SPBRG`:

$$
\
SPBRG + 1 = \frac{16 \times 10^6}{64 \times 9600} = \frac{16 \times 10^6}{614400} \approx 26.04
\
$$

$$
\
\boxed{SPBRG = 25}
\
$$

Por eso se configura:

```c 
SPBRG1 = 25;
```

**¿Qué pasa si se usan BRGH = 1 o BRG16 = 1?**

- Se pueden generar baudrates más precisos o más altos

- Requiere ajustar la fórmula (diferentes divisores)

- Pero en este caso, se elige la configuración más simple y suficiente para 9600 bps.


#### **Ejemplo de cálculo con `BRGH`**

**Supongamos:**
- Frecuencia de reloj $F_{osc}$ = 16 MHz  
- Queremos `baudrate` = 9600


#### **Caso 1: `BRGH = 0`**

$$
\
Baudrate = \frac{F_{osc}}{64 \times (SPBRG + 1)} \Rightarrow SPBRG = \frac{F_{osc}}{64 \times Baudrate} - 1
\
$$

$$
\
SPBRG = \frac{16\,000\,000}{64 \times 9600} - 1 \approx 25
\
$$

#### **Caso 2: `BRGH = 1`**

$$
\
Baudrate = \frac{F_{osc}}{16 \times (SPBRG + 1)} \Rightarrow SPBRG = \frac{F_{osc}}{16 \times Baudrate} - 1
\
$$

$$
\
SPBRG = \frac{16\,000\,000}{16 \times 9600} - 1 \approx 103
\
$$

Ambos valores producen el mismo baudrate, pero con diferentes divisores, lo cual puede afectar:

- La precisión del baudrate real

* La capacidad máxima de velocidad que puede alcanzar el UART

### **Activación del módulo UART y su configuración**

```c 
RCSTA1bits.SPEN = 1; // Habilita módulo serial (activa RC6 y RC7)
TXSTA1bits.SYNC = 0; // Modo asíncrono
TXSTA1bits.TXEN = 1; // Habilita transmisión (TX)
RCSTA1bits.CREN = 1; // Habilita recepción continua (RX)
```

Estas instrucciones encienden el módulo UART del PIC18F45K22, lo configuran en modo asíncrono (el más común para comunicación serial), y habilitan transmisión y recepción de datos.

**`RCSTA1bits.SPEN = 1;`**

**(Serial Port Enable)**

Este bit activa el módulo EUSART (Enhanced Universal Synchronous Asynchronous Receiver Transmitter) y conecta los pines TX (RC6) y RX (RC7) al periférico.

  - `SPEN = 1`: activa el UART

  - `SPEN = 0`: UART desactivado (los pines RC6/RC7 funcionan como GPIO)

Importancia: si este bit está en 0, el UART no funcionará, aunque todo lo demás esté bien configurado.

**`TXSTA1bits.SYNC = 0;`**

**(Synchronous/Asynchronous Mode Select)**

Este bit define si el módulo funciona en modo:

 - SYNC = 0: modo asíncrono (lo que normalmente se usa con PCs, terminales, etc.)

- SYNC = 1: modo síncrono maestro/esclavo (menos común)

Asíncrono significa que no se usa una señal de reloj externa: solo se transmiten los datos por TX y se reciben por RX.

**`TXSTA1bits.TXEN = 1;`**

**(Transmitter Enable bit)**

Este bit habilita el transmisor UART. Sin él, no se puede enviar datos a través de TX.

 - `TXEN = 1`: se activa la lógica de transmisión

 - `TXEN = 0`: el transmisor está apagado

Nota: Aunque el pin TX esté configurado como salida (TRISC6 = 0), no saldrá nada si TXEN está apagado.

**`RCSTA1bits.CREN = 1;`**

**(Continuous Receive Enable bit)**

Este bit habilita la recepción continua de datos UART. Es esencial para que el módulo reciba correctamente sin detenerse después de un byte.

 - `CREN = 1`: recepción continua habilitada

 - `CREN = 0`: se detiene la recepción (por ejemplo, se puede usar en modo manual)

Si se quiere que el UART detecte todos los bytes que llegan por RX, este bit debe estar activado.

**¿Por qué son importantes estos bits?**

Estos registros (`RCSTA1` y `TXSTA1`) controlan el comportamiento del UART. Si cualquiera de estos bits está mal configurado:

 - No se podrá transmitir o recibir

 - El UART puede quedar en modo incorrecto (síncrono en vez de asíncrono)

 - Los pines pueden no estar conectados al módulo UART

### **Interrupciones UART**

```c 
PIE1bits.RC1IE = 1;   // Habilita interrupción por recepción UART
PIR1bits.RC1IF = 0;   // Limpia bandera de recepción
INTCONbits.PEIE = 1;  // Habilita interrupciones periféricas
INTCONbits.GIE = 1;   // Habilita interrupciones globales
```

Configura el sistema de interrupciones del microcontrolador para que interrumpa automáticamente al recibir un byte por UART, en lugar de tener que estar constantemente revisando (polling) si llegó algo por RX.

**`PIE1bits.RC1IE = 1;`**

**(Peripheral Interrupt Enable for EUSART Receive)**

Este bit activa la interrupción específica del UART por recepción.

 - `RC1IE = 1`: se genera una interrupción cuando llega un byte por RX

 - `RC1IE = 0`: no se generará ninguna interrupción aunque llegue un dato

Nota: Este bit solo permite la interrupción si la bandera RC1IF se activa.

**`PIR1bits.RC1IF = 0;`**

**(Receive Interrupt Flag)**

Esta bandera se pone automáticamente en 1 cuando llega un dato por RX.

- Se debe limpiar manualmente (RC1IF = 0) después de leer el dato, para que el sistema reconozca futuras interrupciones.

Aunque normalmente se limpia dentro de la ISR (función de interrupción), aquí se pone en 0 por precaución antes de comenzar.

**`INTCONbits.PEIE = 1;`**

**(Peripheral Interrupt Enable)**

Este bit habilita todas las interrupciones de periféricos, como:

- UART

- ADC

- CCP

- MSSP, etc.

Si está en 0, las interrupciones del UART no funcionarán, aunque RC1IE esté habilitado.

**`INTCONbits.GIE = 1;`**

**(Global Interrupt Enable)**

Este bit activa todas las interrupciones del sistema (tanto de periféricos como externas).

 - `GIE = 1`: habilita las interrupciones globales

 - `GIE = 0`: ninguna interrupción ocurre, aunque estén habilitadas individualmente

Este bit se debe mantener en 1 mientras se quiera que el microcontrolador reaccione a interrupciones.

#### **¿Cómo funciona todo junto?**

Cuando el UART recibe un byte por el pin RX:

1. El hardware pone PIR1bits.RC1IF = 1.

2. Como PIE1bits.RC1IE = 1, y PEIE = 1, y GIE = 1, se genera una interrupción.

3. El micro salta a la función de interrupción (ISR), donde tú puedes leer RCREG1 (registro que contiene el byte recibido).

4. Luego limpias RC1IF para permitir que se dispare una nueva interrupción.

#### **¿Qué pasaría si no hubieran interrupciones?**

1. El microcontrolador tendría que revisar constantemente si llegó un dato
  Se usaria algo como:

```c 
if (PIR1bits.RC1IF) {
    char dato = RCREG1;
    PIR1bits.RC1IF = 0; // Limpiar bandera manualmente
}
```

Esto implica estar en un bucle verificando RC1IF, lo que:

- Consume tiempo de CPU.

- Impide que el micro haga otras tareas de forma eficiente.

- Puede hacer que se pierdan datos si no se lee RCREG1 a tiempo.

#### **Alto consumo de recursos**

- El micro debe dedicar tiempo constantemente a verificar si hay datos nuevos.

- Si el sistema necesita hacer múltiples tareas (lectura de sensores, control de motores, etc.), se vuelve poco práctico y propenso a errores.

**Riesgo de perder datos**

- Si llega un nuevo byte mientras el micro está ocupado en otra cosa, y no se lee a tiempo, se puede sobrescribir el buffer de recepción (RCREG1), provocando pérdida de datos.

**No hay respuesta inmediata**

- En un sistema con interrupciones, la CPU interrumpe lo que está haciendo y atiende el dato entrante inmediatamente.

- Sin interrupciones, el micro puede tardar en leer el dato y reaccionar.


En conclusión sin interrupciones, el sistema UART es menos eficiente y menos confiable, sobre todo cuando se requiere que el microcontrolador haga múltiples cosas a la vez. Las interrupciones son esenciales en sistemas embebidos complejos o con comunicaciones rápidas.

### **UART_WriteChar(char data)**

```c 
while (!TXSTA1bits.TRMT); // Espera hasta que el buffer de transmisión esté vacío
TXREG1 = data;            // Carga el dato a transmitir
```
Esta función envía un solo carácter por UART. Veamos paso a paso lo que hace y cómo se relaciona con los registros del microcontrolador.

**`while (!TXSTA1bits.TRMT);`**

- `TXSTA1`: es el registro Transmitter Status and Control, que controla y monitorea el estado del transmisor UART.

- `TRMT` (Transmit Shift Register Empty):

  - Bit que indica si el registro de desplazamiento de transmisión está vacío.

  - `1:` el registro está vacío, se puede cargar un nuevo dato.

  - `0:` está ocupado transmitiendo un dato.

- El while espera hasta que UART esté listo para enviar otro dato.

- Esto evita sobrecargar el transmisor (enviar un nuevo dato antes de que el anterior haya terminado de salir).

**Importante:** no confundir con TXIF, que indica si TXREG1 está listo para recibir un dato, pero no necesariamente si el dato ya fue transmitido por completo.

**`TXREG1 = data;`**

- `TXREG1` es el registro de transmisión, donde se carga el dato que se quiere enviar.

- Una vez se escribe aquí, el dato se transfiere automáticamente al registro de desplazamiento, que es el que realmente transmite el bit por bit.

- El microcontrolador se encarga de enviar el dato por el pin TX (RC6) usando el protocolo UART.

**¿Qué hace internamente el hardware?**

1. Se escribe un byte en `TXREG1`.

2. El hardware espera a que el canal esté libre (`TRMT = 1`).

3. El dato se transfiere al registro de desplazamiento, y `TXREG1` queda listo para otro dato.

4. El dato es transmitido bit a bit, incluyendo el bit de inicio, los 8 bits de datos, y el bit de parada.

5. Una vez completado, `TRMT` vuelve a 1, indicando que se puede enviar otro.

**¿Por qué esperar con while (!TRMT)?**

- Asegura que cada carácter se transmite completamente antes de mandar el siguiente.

- Si envías varios caracteres rápidamente (como en UART_WriteString()), esto evita errores de sobrecarga del buffer.

### **`UART_WriteString(const char* str)`**

```c 
void UART_WriteString(const char* str) {
    while (*str) {
        UART_WriteChar(*str++);
    }
}
```

Esta función se encarga de enviar una cadena de caracteres (es decir, un texto completo) por UART, carácter por carácter, usando la función UART_WriteChar().

**`¿Qué es const char* str?`**

- Es un puntero a una cadena de caracteres terminada en `\0`.

- const: indica que la función no modificará la cadena apuntada.

- Ejemplo:

```c 
UART_WriteString("Hola!");
```

Internamente, "Hola!" está representado así en memoria:

```c 
'H' → 'o' → 'l' → 'a' → '!' → '\0'
```
**`while (*str)`**

- `str` evalúa el contenido apuntado por `str`.

- El bucle se ejecuta mientras no se encuentre el carácter nulo '\0', que marca el final de la cadena en C.

- Cuando se encuentra '\0', el bucle termina.

**`UART_WriteChar(*str++)`**

- `str` obtiene el carácter actual apuntado por el puntero.

- `UART_WriteChar()` lo envía por UART (como se explicó antes).

- Luego, str++ avanza el puntero al siguiente carácter de la cadena.

## **Archivo `uart.h`**

Define las funciones públicas y la frecuencia de reloj:

```c 
#define _XTAL_FREQ 16000000UL // Necesario para __delay_ms()
```

Declara las funciones:

```c 
void UART_Init(void);
void UART_WriteChar(char data);
void UART_WriteString(const char* str);
```

Las funciones `UART_WriteUInt` y `UART_WriteVoltage` están declaradas pero no implementadas (Estos servira para la Parte 2 de este Laboratorio).

**ENLACE A CODIGOS `newmain`, `uart,h` y `uart.h`.**

| Archivo          | Descripción           | Tipo       |
|------------------|-----------------------|------------|
| [newmain](/CODIGOS_PARTE_1/main.c)  | Archivo Principal | C        |
| [uart.h](/CODIGOS_PARTE_1/uart.h)   | Archico encabezado    | h     |
| [uart.c](/CODIGOS_PARTE_1/uart.c)   | Declaración de variables     | C     |

## **Conexión con PuTTY para Comunicación UART**

### **¿Qué es PuTTY?**

PuTTY es una herramienta gratuita para Windows que permite la comunicación serial (entre otras). En este contexto, se uso para recibir y visualizar los datos enviados desde el microcontrolador a través del puerto UART (por ejemplo: "Hola, UART funcionando!").

### **Requisitos previos**

- Cable USB-UART (como el conversor USB a TTL basado en CH340, FT232, PL2303, etc.)

- Conexión física correcta entre el PIC18F45K22 y el conversor UART

- Driver del adaptador USB-UART instalado en tu PC

- PuTTY instalado

### **Configuración en PuTTY**

1. Se abre PuTTY

![DIAGRAMA](/IMAGENES/Abrir%20Putty.png)

2. Selecciona "Serial" como tipo de conexión

![alt text](/IMAGENES/Serial_Putty.png)

3. En "Serial line", escribe el puerto COM asignado (ejemplo: COM5)

![alt text](/IMAGENES/Putty_COM.png)

4. Se puede ver en el Administrador de dispositivos de Windows

![alt text](/IMAGENES/PUTTY_COM5.png)

5. En "Speed" (velocidad), escribe 9600 (porque el código está configurado a 9600 baudios)

![alt text](/IMAGENES/Velocidad_PUTTY.png)

6. Haz clic en "Open"

### **Comunicación en tiempo real**

Una vez abierto PuTTY:

- Deberia comenzar a ver el mensaje "Hola, UART funcionando!" repitiéndose cada 1 segundo.

![alt text](/IMAGENES/uart_funcionando.png)

- Si no aparece nada:

  - Revisar la conexión TX/RX

  - Verificar que el COM sea el correcto

  - Asegúrarse de que PuTTY no esté abierto en otro programa



## Descripción del Código PARTE 2 

Dado que la mayoría de los conceptos fundamentales fueron abordados en la Parte 1, esta Parte 2 se centrará en desarrollar nuevos conceptos y sintetizar aquellos previamente explicados. Si surge alguna duda respecto a un concepto, función o registro previamente discutido, se recomienda revisar nuevamente la [Parte 1](#descripción-del-código-parte-1) para reforzar el entendimiento.

## **Archivo `newmain.c`**

Este es el archivo principal del programa que corre en el PIC. Controla la lógica principal: inicialización, lectura del ADC, conversión a voltaje y envío por UART.

```c 
#include <xc.h>
#include <stdio.h>
#include "ADC.h"
#include "UART.h"
#define _XTAL_FREQ 16000000
```

- `#include <xc.h>`: Librería del compilador XC8 que permite el acceso a los registros del PIC.

- `#include <stdio.h>`: Necesario para sprintf, que convierte el voltaje flotante a texto.

- `#define _XTAL_FREQ`: Define la frecuencia del reloj interno para funciones como __delay_ms.

### **Configuración del PIC:**

```c 
#pragma config FOSC = INTIO67
#pragma config WDTEN = OFF
#pragma config LVP = OFF
```

- `FOSC = INTIO67`: Usa el oscilador interno de 16 MHz.

- `WDTEN = OFF`: Desactiva el watchdog timer.

- `LVP = OFF`: Desactiva la programación en bajo voltaje (no se necesita).

### **Función mian()**

```c 
void main(void) {
    OSCCON = 0x72;     // Configura el oscilador a 16 MHz
    UART_Init();       // Inicializa UART
    ADC_Init();        // Inicializa ADC

    while (1) {
        // Bucle principal...
    }
}
```
#### **`OSCCON = 0x72;`**

Esta instrucción configura el oscilador interno del PIC18F45K22 para que funcione a 16 MHz. Veamos cómo se interpreta 0x72 en binario:

Los bits del registro `OSCCON` tienen funciones específicas:

| Bit   | Nombre      | Descripción                                                        | Valor |
|-------|-------------|---------------------------------------------------------------------|--------|
| 7     | IDLEN       | Modo IDLE activado o no (no lo usamos aquí)                         | 0      |
| 6–4   | IRCF<2:0>   | Selección de frecuencia del oscilador interno                       | 111    |
| 3     | OSTS        | Indica si el oscilador primario está activo (solo lectura)          | x      |
| 2     | HFIOFS      | Frecuencia estable (solo lectura)                                   | x      |
| 1     | SCS1        | Sistema de reloj: 1 = usa oscilador interno                         | 1      |
| 0     | SCS0        | Sistema de reloj: 0 = usa la configuración predeterminada           | 0      |

Con `0x72 = 01110010`, se están haciendo estas configuraciones:

- `IRCF = 111`: Selecciona 16 MHz como frecuencia del oscilador interno.

- `SCS = 10`: Selecciona el oscilador interno HFINTOSC como fuente del sistema de reloj.

### **`UART_Init();`**

Esta función está definida en el archivo UART.c. Su función es configurar la comunicación UART (Universal Asynchronous Receiver-Transmitter), que permite al PIC comunicarse con un PC o cualquier dispositivo serial.

**¿Qué hace internamente UART_Init()? (Esta es una explicación sintetizada, si necesita una explicación mas detallada consulte en: [Configuración_del_Reloj](#configuración-del-reloj)):**

- Configura pines TX (transmisión) y RX (recepción).

- Establece velocidad de transmisión a 9600 bps con SPBRG1 = 25.

- Configura los modos de operación de UART:

  - Asíncrono.

  - Habilita transmisión y recepción.

- Habilita interrupciones para recibir datos (aunque en este caso, no se usan aún).

### **`ADC_Init();`**

Esta función está definida en el archivo ADC.c y se encarga de configurar el convertidor analógico-digital (ADC) del PIC para que pueda leer señales analógicas (como la de un potenciómetro).

**¿Qué hace internamente ADC_Init()?**

- Configura el pin RA0/AN0 como entrada analógica.

- Habilita el módulo ADC (ADCON0).

- Define justificación, tiempo de adquisición y fuente de reloj del ADC.

### **¿Qué logra esta parte de main()?**

Prepara al microcontrolador para:

- Trabajar a 16 MHz con el oscilador interno (preciso y rápido).

- Enviar datos por UART a un computador (por ejemplo, a Python vía USB).

- Leer valores analógicos por el pin AN0 (como un potenciómetro o sensor).

### **`Bucle while(1)`**

```c 
while (1) {
    unsigned int adc_value = ADC_Read(0); // Lee el canal AN0
    float voltage = (adc_value * 5.0) / 1023.0; // Conversión a voltaje
    char buffer[20];
    sprintf(buffer, "Voltaje: %.2fV\r\n", voltage); // Formatea la cadena
    UART_WriteString(buffer); // Envía la cadena por UART
    __delay_ms(1000); // Espera 1 segundo
}
```
Este bucle es el ciclo principal del programa y se ejecuta infinitamente. Su función es:

- Leer un voltaje analógico,

- Convertirlo a formato de texto legible, y

- Enviarlo por UART cada segundo.

### **`unsigned int adc_value = ADC_Read(0);`**

Esta línea de código realiza la lectura del valor digital generado por el módulo ADC (Convertidor Analógico a Digital) del microcontrolador PIC18F45K22, desde el canal analógico 0 (AN0).

### **`ADC_Read(0) — Llamada a la función de lectura ADC`**

Esta función está definida en ADC.c, y su propósito es:

- Configurar el canal analógico solicitado (en este caso el canal 0),

- Iniciar una conversión analógica a digital, y

- Retornar el resultado de la conversión como un número entre 0 y 1023.

#### **¿Qué significa el parámetro 0?**

- El número 0 indica que se desea leer el canal AN0, que corresponde al pin RA0 del microcontrolador.

- Esto es útil cuando hay múltiples entradas analógicas y se quiere seleccionar una en particular.

### **¿Qué hace internamente `ADC_Read()?`**

La función definida en el archivo `ADC.c` hace lo siguiente:

```c 
unsigned int ADC_Read(unsigned char canal) {
    ADCON0 &= 0xC5;             // Limpia los bits del canal anterior
    ADCON0 |= (canal << 2);     // Selecciona el canal deseado
    __delay_ms(2);              // Espera tiempo de adquisición
    ADCON0bits.GO = 1;          // Inicia la conversión
    while (ADCON0bits.GO);      // Espera a que termine
    return ((unsigned int)(ADRESH << 8) | ADRESL); // Retorna resultado
}
```
Explicación:

- `ADCON0 &= 0xC5;`: Borra bits de canal anterior en el registro ADCON0.

- `ADCON0 |= (canal << 2);`: Inserta el número del canal deseado.

- `__delay_ms(2);`: Tiempo de adquisición (establecer el voltaje internamente).

- `ADCON0bits.GO = 1;`: Inicia la conversión ADC.

- `while (ADCON0bits.GO);`: Espera a que la conversión termine.

- `return ((ADRESH << 8) | ADRESL);`: Combina los 2 registros de resultado (alto y bajo) en un solo entero de 10 bits.

**¿Qué hace la variable `adc_value`?**

Almacena el resultado de la lectura del ADC.
Luego se usa en:

```c 
float voltage = (adc_value * 5.0) / 1023.0;
```
para convertir ese valor digital a voltaje real (en voltios).

### **`float voltage = (adc_value * 5.0) / 1023.0;`**

- Convierte el valor digital del ADC a un **voltaje real en voltios**.

#### Fórmula:

$$
Voltaje = \frac{ADC{value * 0.5}}{1023.0}
$$


#### **¿Qué hace esta fórmula?**

El ADC del **PIC18F45K22** es de **10 bits**, lo que significa que puede representar \( 2^{10} = 1024 \) **niveles discretos**, numerados del 0 al 1023.

Cada uno de estos niveles representa una fracción del voltaje de referencia (que en este caso es 5.0 V):

$$\text{Resolución del ADC} = \frac{5.0\ \text{V}}{1023} \approx 0.00489\ \text{V por unidad}\ $$

Entonces, para convertir un valor del ADC a voltaje real, se multiplica el valor digital por esa fracción:

$$
Voltaje = \frac{ADC{value * 0.5}}{1023.0}
$$

### Ejemplos:

| `adc_value` | Voltaje calculado (aprox) |
|-------------|---------------------------|
| 0           | 0.00 V                    |
| 511         | ≈ 2.50 V                  |
| 1023        | 5.00 V                    |

### **`sprintf(buffer, "Voltaje: %.2fV\r\n", voltage);`**

Esta instrucción convierte el valor de voltaje (un número flotante) en una cadena de texto con formato, para luego poder enviarla por UART. 

**`¿Qué es sprintf?`**

sprintf es una función de la biblioteca estándar de C (stdio.h) que formatea texto y lo guarda en una cadena (buffer), en lugar de imprimirlo en pantalla (como hace printf).

**Componentes de esta línea**

- `buffer`: Es un arreglo de tipo char (ej. char buffer[20];) donde se almacena el texto generado.

- `"Voltaje: %.2fV\r\n"`: Es el formato de texto que se va a generar.

- `voltage`: Es el valor de tipo float que será insertado en el lugar de `%.2f`.

**¿Qué hace "Voltaje: `%.2fV\r\n"?`**

- `"Voltaje:"`: Texto fijo.

- `%.2f`: Especificador de formato para un número decimal (float) con 2 cifras después del punto decimal.

- `"V"`: Añade la unidad “Voltios” al final.

- `\r\n`: Es el retorno de carro y salto de línea, requerido por muchos programas terminal (como PuTTY) para iniciar una nueva línea correctamente.

**¿Por qué se usa esta cadena formateada?**

Porque luego esta cadena se envía por UART con UART_WriteString(buffer);, y es exactamente la cadena que el programa en Python recibirá, procesará con regex, y graficará.

### **`UART_WriteString(buffer);`**

Esta línea se encarga de enviar por UART el texto que se generó previamente con sprintf, es decir, algo como `"Voltaje: 3.27V\r\n"`.

**¿Qué es `UART_WriteString?`**

Es una función definida por el usuario en el archivo UART.c, cuya finalidad es enviar una cadena de caracteres (texto) por el puerto UART, carácter por carácter.

**¿Cómo funciona?**

- `const char* str`: Es un puntero al primer carácter de la cadena (por ejemplo, "Voltaje: 3.27V\r\n").

- `while (*str)`: Mientras no se llegue al final de la cadena (cuando el carácter sea '\0'), continúa el ciclo.

- `UART_WriteChar(*str++)`: Envía el carácter actual por UART y avanza al siguiente carácter.

**¿Qué hace internamente?**

Llama muchas veces a la función UART_WriteChar, que envía un solo carácter por UART:

```c 
void UART_WriteChar(char data) {
    while (!TXSTA1bits.TRMT);  // Espera a que el buffer de transmisión esté listo
    TXREG1 = data;             // Envía el carácter
}
```
**Resultado final**

Al ejecutar `UART_WriteString(buffer);`, se envía por UART el siguiente texto completo (por ejemplo): 

```c 
Voltaje: 3.27V⏎ (retorno de carro y nueva línea)
```
## **ARCHIVOS `UART.h` y `UART.c`**

Los archivos [UART.h](#archivo-uarth) y [UART.c](#archivo-uartc--implementación-de-la-comunicación-uart) que se estan utilizando en esta segunda parte del proyecto son los mismos que se explicaron anteriormente, tanto en estructura como en funcionalidad.

## **ARCHIVOS `ADC.h` y `ADC.c`**

### **Archivo: `ADC.h`**

Este archivo contiene el encabezado del módulo ADC, donde se definen las funciones que luego se implementan en ADC.c.

```c 
#ifndef ADC_H
#define ADC_H

#include <xc.h>

// Prototipos de funciones
void ADC_Init(void);
unsigned int ADC_Read(unsigned char canal);

#endif // ADC_H
```

- `#ifndef,` `#define,` `#endif`: Son directivas para evitar que el archivo se incluya más de una vez.

- `#include <xc.h>`: Incluye las definiciones específicas del compilador XC8 y del microcontrolador.

- **Prototipos:**

  - `void ADC_Init(void);`: inicializa el módulo ADC.

  - `unsigned int ADC_Read(unsigned char canal);`: realiza una lectura en el canal analógico especificado y devuelve un valor de 10 bits (0–1023).

### **Archivo: `ADC.c`**

Este archivo contiene la implementación del módulo ADC. Aquí se inicializa el ADC y se realiza la lectura analógica ( Esta explicación es sintetizada, es decir solo se limita a explicar los registros de fomar mas precisa, si desea saber a fondo el funcionamiento de estos archivos puede ir a [Repositorio Lab03 - PWM Grupo 1 Equipo 3](https://github.com/ECCI-microprocesadores/lab03-pwm-g1-e3/blob/main/README.md)
en la sección C**onfiguración del PWM y Control de Ciclo Útil**).

```c 
#include <xc.h>
#include "ADC.h"

#define _XTAL_FREQ 16000000

void ADC_Init(void) {
    TRISAbits.TRISA0 = 1;       // Configura AN0 como entrada
    ANSELAbits.ANSA0 = 1;       // Configura AN0 como canal analógico

    ADCON0 = 0x01;              // Habilita el módulo ADC, selecciona canal AN0
    ADCON1 = 0x00;              // Justificación izquierda (no se usa en este caso)
    ADCON2 = 0xA9;              // Tiempo de adquisición 8 TAD, Fosc/8
}
```

### **`ADC_Init():`**

- `TRISAbits.TRISA0 = 1;`: configura el pin RA0 (AN0) como entrada digital.

- `ANSELAbits.ANSA0 = 1;`: convierte RA0 en entrada analógica.

- `ADCON0 = 0x01;`: selecciona el canal AN0 y activa el ADC.

- `ADCON1 = 0x00;`: configuración de voltajes de referencia (usa Vdd y Vss).

- `ADCON2 = 0xA9;`: configuración del tiempo de adquisición y del reloj del ADC.

  - `Tacq = 8 TAD`: tiempo para cargar el capacitor interno antes de leer.

  - `Fosc/8`: frecuencia de reloj del ADC para obtener conversiones estables.

### **`ADC_Read(canal)`**

```c 
unsigned int ADC_Read(unsigned char canal) {
    ADCON0 &= 0xC5;             // Limpia los bits del canal anterior
    ADCON0 |= (canal << 2);     // Selecciona el nuevo canal
    __delay_ms(2);              // Tiempo de adquisición (espera 2 ms)
    ADCON0bits.GO = 1;          // Inicia la conversión
    while (ADCON0bits.GO);      // Espera a que la conversión termine

    return ((unsigned int)(ADRESH << 8) | ADRESL); // Une los 2 bytes (10 bits)
}
```

- `canal`: número del canal analógico que se quiere leer (ej. 0 = AN0).

- `ADCON0 &= 0xC5;`: limpia los bits que seleccionan el canal.

- `ADCON0 |= (canal << 2);`: coloca el nuevo canal en los bits adecuados.

- `__delay_ms(2);`: da tiempo para que el ADC estabilice la señal.

- `ADCON0bits.GO = 1;`: inicia la conversión.

- `while (ADCON0bits.GO);`: espera a que la conversión termine (cuando GO = 0).

- `return ((ADRESH << 8) | ADRESL);`: los 10 bits del resultado vienen divididos en dos registros:

  - `ADRESH`: bits altos (2).

  - `ADRESL`: bits bajos (8).

  - Se combinan usando un desplazamiento y una operación OR.

### **Resultado final:**

La función devuelve un número entero de 0 a 1023, que representa la señal analógica leída (por ejemplo, de un potenciómetro).

## **Código Python**

### **¿Qué hace el código de Python?**

El script en Python recibe los datos enviados por el PIC18F45K22 a través de UART, los interpreta como voltajes (ya convertidos por el PIC a partir de la señal analógica), y grafica en tiempo real esos valores.

### **Flujo general del código Python**

- Se conecta al puerto serial (COM) donde el PIC está enviando datos.

- Lee los valores de voltaje transmitidos por el PIC (en formato texto).

- Guarda esos valores en una lista que se va actualizando.

- Dibuja una gráfica en tiempo real que se actualiza cada segundo (1000 ms).

### **Función del código paso a paso**

### **Importación de librerías**

```python
import serial
import matplotlib.pyplot as plt
import matplotlib.animation as animation
import re
from collections import deque
```
- `serial`: Módulo de pyserial que permite comunicación por puerto serial (UART).

- `matplotlib.`: Librería para crear gráficos.

- `matplotlib.animation`: Permite animaciones (actualizar el gráfico en tiempo real).

- `re`: Librería de expresiones regulares (para extraer el número del texto).

- `deque`: Estructura de datos que permite manejar una lista de longitud fija (ventana deslizante).

### **Configuración del puerto y variables**

```python
SERIAL_PORT = '/dev/ttyUSB0'     
BAUDRATE = 9600

MAX_POINTS = 100

ser = serial.Serial(SERIAL_PORT, BAUDRATE, timeout=1)

voltages = deque(maxlen=MAX_POINTS)
times = deque(maxlen=MAX_POINTS)
time_counter = 0

regex = re.compile(r"Voltaje:\s*([0-9.]+)")
```
- `SERIAL_PORT`: Puerto USB donde está conectado el microcontrolador.

  - En Windows sería algo como 'COM3'.

- `BAUDRATE`: Velocidad de comunicación (debe coincidir con el PIC, que es 9600).

- `ser`: Objeto que representa la conexión serial con el microcontrolador.

- `voltages`: Lista de voltajes leídos (máximo 100 elementos).

- `times`: Lista de tiempos (eje X del gráfico).

- `regex`: Expresión regular que busca texto como "Voltaje: 3.21V" y extrae 3.21.

### **Función que actualiza el gráfico**

```python
def update(frame):
    global time_counter
    line = ser.readline().decode('utf-8').strip()
    
    match = regex.search(line)
    if match:
        voltage = float(match.group(1))
        voltages.append(voltage)
        times.append(time_counter)
        time_counter += 1

        ax.clear()
        ax.plot(times, voltages, color='blue')
        ax.set_ylim(0, 5)
        ax.set_title("Voltaje leído por UART")
        ax.set_xlabel("Tiempo (s)")
        ax.set_ylabel("Voltaje (V)")
        ax.grid(True)
```
### **Función `update(frame)`**

```python
def update(frame):
    global time_counter
```

- `update(frame)`: Esta función es llamada por FuncAnimation cada cierto intervalo (en este caso, cada 1000 ms = 1 segundo).

- `global time_counter`: Se declara como global para que la variable time_counter se actualice y retenga su valor entre llamadas. Esta variable representa el tiempo (en segundos) en el eje X del gráfico.

```python
    line = ser.readline().decode('utf-8').strip()
```
- `ser.readline()`:

  - Lee una línea completa del puerto serial (hasta \n).

  - Por ejemplo, el microcontrolador envía:
    "Voltaje: 2.57V\r\n"

- `.decode('utf-8')`: Convierte los bytes leídos desde UART en una cadena de texto.

- `.strip()`: Elimina espacios y saltos de línea (\r, \n), dejando solo:
"Voltaje: 2.57V"

```python
    match = regex.search(line)
```

- `regex` es una expresión regular que busca el patrón "Voltaje: <número>".

- Esta línea busca ese patrón dentro de la cadena `line`.

- Si encuentra algo como `"Voltaje: 3.14V"`, el resultado es un objeto `match`.

```python
    if match:
        voltage = float(match.group(1))
```
- Si match no es None, entonces:

  - `match.group(1)` devuelve la parte numérica del voltaje, por ejemplo '3.14'.

  - `float()` convierte ese texto en un número decimal 3.14.

```python
        voltages.append(voltage)
        times.append(time_counter)
        time_counter += 1
```

- `voltages.append(voltage)`:

  - Añade el valor del voltaje al final de la lista voltages.

  - Como es un `deque(maxlen=100)`, si ya hay 100 valores, elimina el más antiguo automáticamente.

- `times.append(time_counter)`:

  - Agrega el contador de tiempo para que se pueda graficar el voltaje contra el tiempo.

- `time_counter += 1`:

  - Aumenta el tiempo (1 segundo por iteración) para el próximo punto del eje X.


```python
        ax.clear()
        ax.plot(times, voltages, color='blue')
```
- `ax.clear()`: Limpia el gráfico actual para dibujar uno nuevo actualizado.

- `ax.plot(times, voltages, color='blue')`: Grafica los valores del voltaje vs tiempo.

```python
        ax.set_ylim(0, 5)
        ax.set_title("Voltaje leído por UART")
        ax.set_xlabel("Tiempo (s)")
        ax.set_ylabel("Voltaje (V)")
        ax.grid(True)
```

- `ax.set_ylim(0, 5)`: Limita el eje Y entre 0 y 5V (máximo que puede leer el ADC).

- `ax.set_title(...)`, `set_xlabel(...)`, `set_ylabel` Etiquetas y título del gráfico.

- `ax.grid(True)`: Muestra una cuadrícula para facilitar la lectura del gráfico.

### **Crear y mostrar el gráfico en tiempo real**

```python
fig, ax = plt.subplots()
ani = animation.FuncAnimation(fig, update, interval=1000, cache_frame_data=False)
plt.tight_layout()
plt.show()
```

```python
fig, ax = plt.subplots()
Esta línea crea una figura (fig) y un eje (ax) para graficar.
```

Es equivalente a hacer:
```python
fig = plt.figure()
ax = fig.add_subplot(1, 1, 1)
```
- `fig`: el contenedor principal del gráfico.

- `ax`: es el "cuadro" dentro del cual se dibuja el gráfico de voltaje.

- Este `ax` es el que se utiliza en la función `update()` para graficar `(ax.plot(...))`.

### **`ani = animation.FuncAnimation(...)`**

```python
ani = animation.FuncAnimation(fig, update, interval=1000, cache_frame_data=False)
```

- Crea una animación que actualiza el gráfico en tiempo real.

**Parámetros:**

- `fig`: la figura donde se dibuja el gráfico.

- `update`: la función que se llama cada vez para actualizar el gráfico (cada segundo).

- `interval=1000`: define que se llama update() cada 1000 ms (1 segundo).

- `cache_frame_data=False`: evita que se guarden en memoria los cuadros anteriores. Esto es útil porque los datos cambian constantemente y no se necesita guardar los anteriores (ahorra memoria).

Resultado: Se ejecuta update() cada segundo, leyendo nuevos datos del puerto serial y redibujando el gráfico.

### **`plt.tight_layout()`**

- Ajusta automáticamente los márgenes de la figura para que:

  - Los títulos, etiquetas y ejes no se solapen ni salgan del área visible.

- Es útil cuando tienes etiquetas largas o muchos elementos en el gráfico.

### **`plt.show()`**

- Muestra la ventana del gráfico.

- Se queda abierta mientras se recibe información y se actualiza el gráfico en tiempo real.

- Es una llamada bloqueante, es decir, el programa se queda aquí y no continúa hasta que se cierre la ventana del gráfico.

**ENLACE A CODIGOS `newmain`, `UART,h` y `UART.h`.**

| Archivo          | Descripción           | Tipo       |
|------------------|-----------------------|------------|
| [newmain](/CODIGOS_PARTE_2/newmain.c)  | Archivo Principal | C        |
| [UART.h](/CODIGOS_PARTE_2/UART.h)   | Archico encabezado    | h     |
| [UART.c](/CODIGOS_PARTE_2/UART.c)   | Declaración de variables     | C     |

**ENLACE A CODIGOS `Python`, `ADC,h` y `ADC.c`.**

| Archivo          | Descripción           | Tipo       |
|------------------|-----------------------|------------|
| [ADC.c](/CODIGOS_PARTE_2/ADC.c)  | Declaración de Variables | C        |
| [ADC.h](/CODIGOS_PARTE_2/ADC.h)   | Archico encabezado    | h     |
| [serial_pic.py](/CODIGOS_PARTE_2/serial_pic.py)   |   Interpretar valores de UART     | py   |

### Resultado 

Como resultado al copilar el codigo de Python deberia verse la siguiente ventana 

![alt text](/IMAGENES/ADC_UART.png)

En la gráfica se observa cómo varía el voltaje leído a través de la comunicación UART en función del tiempo. A medida que se incrementa la resistencia del potenciómetro y se aproxima a su valor máximo, el voltaje también aumenta hasta alcanzar un valor cercano a 5 voltios, lo cual se refleja en el eje vertical (Y). Posteriormente, al disminuir la resistencia del potenciómetro, el voltaje decrece progresivamente hasta aproximarse a 0 voltios. El eje horizontal (X) representa el tiempo transcurrido en segundos, lo que permite visualizar claramente la evolución del voltaje en función del ajuste del potenciómetro. Esta gráfica ilustra de manera efectiva la relación entre el control resistivo del potenciómetro y la señal analógica convertida y transmitida por UART.

### **Diagrama   PIC18F45K22**

![DIAGRAMA](/IMAGENES/DIAGRAMA.png)


## Internal Oscillator Block

## 1. Oscilador Interno 
```c 
#pragma config FOSC = INTIO67  
OSCCON = 0b01110000;  


```
- Ubicación en el diagrama:

- Parte inferior izquierda: Internal Oscillator Block

Función:Configura el sistema de reloj interno del microcontrolador a 16 MHz utilizando el oscilador interno. Esto determina la velocidad de ejecución del programa y periféricos como UART y ADC.


## 2. ADC en RA0 (entrada analógica)
```c 
TRISA |= 0b00000001;   // RA0 como entrada
ANSELA |= 0b00000001;  // Habilita analógico en RA0

ADCON0 = 0b00000001;   // Selecciona canal AN0, ADC ON
ADCON1 = 0b00000000;   // Vref+ = VDD, Vref- = VSS
ADCON2 = 0b10111110;   // Justificado a la derecha, Tacq, Fosc/64


```
## Ubicación en el diagrama:
- Parte inferior derecha: ADC 10-bit

- Asociado a PORTA (RA0)

Función: Configura el pin RA0 como entrada analógica y activa el ADC para leer sensores analógicos.

## 3 Lectura del ADC y conversión a voltaje
```c 
adc = readADC();  
voltaje = adc * 5.0 / 1023.0;


``` 
## Ubicación en el diagrama:
- Parte inferior derecha: ADC 10-bit

Función:Lee el valor convertido del ADC (10 bits) y lo transforma en un voltaje en voltios.
## 4. Comunicación Serial UART
```c 
UART_Init();  
UART_WriteString(buffer);  

```
## Ubicación en el diagrama:
- Parte inferior: EUSART1

- Asociado a PORTC (RC6 = TX, RC7 = RX)

- Asociado con PORTC (RC1, RC2)

Función:Inicializa el módulo UART y envía el voltaje convertido a través del puerto serial (por ejemplo, hacia un monitor serial en PC).


## 5. Configuración de Pines UART
```c 
TRISC6 = 0; // TX como salida  
TRISC7 = 1; // RX como entrada  

```
## Ubicación en el diagrama:
- Parte derecha: PORTC (RC6 y RC7)

Función:Configura los pines TX y RX para la comunicación serial UART.
## 6. Lógica principal del programa
```c 
// Uso de while(1), condiciones, funciones, etc.

```
## Ubicación en el diagrama:
- Ubicación en el diagrama:

- Parte central: bloques Instruction Decode and Control, ALU, Program Counter, etc.

Función: Se utilizan los bloques centrales del CPU para ejecución de instrucciones, operaciones lógicas, comparación y llamadas a funciones.


# DIAGRAMAS 

### CONEXIÓN DE PINES

![alt text](/IMAGENES/CONEXIONES_UART.jpg)

### DIAGRAMA DE FLUJO

![alt text](/IMAGENES/DIAGRAMA_UART.png)

# IMPLEMENTACIÓN

[VIDEO DE IMPLEMENTACIÓN](https://youtu.be/MxzA_0oDXVE)

# REFERENCIAS

(S/f). *Microchip.com*. Recuperado el 12 de abril de 2025, de [https://ww1.microchip.com/downloads/en/DeviceDoc/40001412G.pdf](https://ww1.microchip.com/downloads/en/DeviceDoc/40001412G.pdf) 