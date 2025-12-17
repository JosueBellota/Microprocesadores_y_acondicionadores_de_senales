# Guía de Estudio - Anexos del Examen

Esta guía explica el contenido de los Anexos proporcionados en el examen y cómo utilizarlos correctamente.

---

## Anexo 1: ADC (ESP-IDF)
Este anexo contiene funciones de la librería nativa de Espressif (`esp-idf`) para el convertidor Analógico-Digital.

### Funciones Clave
*   **`adc1_config_width(width)`**: Define la resolución.
    *   *Tip:* A mayor resolución (ej. `ADC_WIDTH_BIT_12`), mayor rango de valores (0-4095), pero puede ser más ruidoso. Para el examen pidieron 11 bits (0-2047).
*   **`adc1_config_channel_atten(channel, atten)`**: Define el rango de voltaje de entrada.
    *   `ADC_ATTEN_DB_0`: ~1.1V máx.
    *   `ADC_ATTEN_DB_11`: ~3.3V máx (La más común para sensores de 3.3V).
*   **`adc1_get_raw(channel)`**: Lee el valor digital actual.

### Puntos Importantes
*   El ESP32 tiene ADC1 y ADC2. El ADC2 no se puede usar si el WiFi está activo. El examen usó ADC1 (seguro).
*   Los GPIOs se mapean a canales (ej. GPIO35 = Channel 7). El anexo suele incluir esta tabla ("Enumerations").

---

## Anexo 2: Timers (ESP-IDF)
Manejo de temporizadores por hardware. Es más complejo que en Arduino.

### Estructura `timer_config_t`
Es vital configurarla bien:
*   `alarm_en`: `TIMER_ALARM_EN` para interrupciones periódicas.
*   `counter_en`: `TIMER_PAUSE` (iniciar parado).
*   `auto_reload`: `TIMER_AUTORELOAD_EN` para que se reinicie solo tras la alarma (periódico).
*   `divider`: Divisor del reloj base (80MHz). Si `divider=80` -> 1 MHz (1 tick = 1us).

### Secuencia de Inicialización (Receta)
1.  Definir configuración (`timer_config_t`).
2.  `timer_init(...)`.
3.  `timer_set_counter_value(...)` (usualmente a 0).
4.  `timer_set_alarm_value(...)` (valor al que queramos que salte).
5.  `timer_enable_intr(...)`.
6.  `timer_isr_register(...)` (vincular la función ISR).
7.  `timer_start(...)`.

### Grupos y Timers
El ESP32 tiene 2 grupos (0 y 1) con 2 timers cada uno (0 y 1).
*   Timer 0 = Group 0, Timer 0.
*   Timer 1 = Group 0, Timer 1.
*   Timer 2 = Group 1, Timer 0.
*   Timer 3 = Group 1, Timer 1.

---

## Anexo 3: UART (Arduino `Serial`)
Muy sencillo comparado con ESP-IDF.

### `Serial.begin(speed, config)`
*   **Speed**: Baudios (ej. 9600, 115200).
*   **Config**: Formato `SERIAL_DPS` (Data, Parity, Stop).
    *   `8N1`: 8 datos, No paridad, 1 stop (Defecto).
    *   `8E2`: 8 datos, Paridad **E**ven (Par), 2 stop.
    *   `8O1`: 8 datos, Paridad **O**dd (Impar), 1 stop.

### Throughput
El "coste" de enviar un byte no son 8 bits. Son:
Start (1) + Datos (N) + Paridad (0/1) + Stop (1/2).
*   Ejemplo 8N1: 1+8+0+1 = 10 bits/byte.
*   Ejemplo 8E2: 1+8+1+2 = 12 bits/byte.

---

## Anexo 4: I2C (Arduino `Wire`)
Comunicación serie síncrona Maestro-Esclavo.

### Direcciones I2C (¡Ojo!)
*   En las hojas de datos, a veces dan la dirección de 8 bits (ej. 0xA0 escritura, 0xA1 lectura).
*   La librería `Wire` usa direcciones de **7 bits**.
*   Para convertir 8-bit a 7-bit: **Desplazar 1 a la derecha** (`0xA0 >> 1 = 0x50`).
*   Si te dan dirección impar (ej. 0xA3), es la dirección de lectura de 8 bits. La de 7 bits es `0xA3 >> 1 = 0x51`.

### Patrones de Uso
1.  **Escribir en registro:**
    `beginTransmission(addr)` -> `write(reg)` -> `write(dato)` -> `endTransmission()`.
2.  **Leer de registro (Repeated Start):**
    `beginTransmission(addr)` -> `write(reg)` -> `endTransmission(false)` (¡Importante el false!) -> `requestFrom(addr, len)` -> `read()`.

---

## Anexo 5: Arduino GPIO e Interrupciones
Básico de Arduino.

*   **`pinMode(pin, mode)`**: `INPUT`, `OUTPUT`, `INPUT_PULLUP`.
*   **`attachInterrupt(digitalPinToInterrupt(pin), ISR, mode)`**:
    *   `mode`: `RISING` (subida), `FALLING` (bajada), `CHANGE`, `LOW`.
    *   La ISR debe ser una función `void func()`. En ESP32 debe tener atributo `IRAM_ATTR`.

---

## Anexo 6: FreeRTOS
Sistema Operativo en Tiempo Real.

### Conceptos Clave
1.  **Tareas (`xTaskCreate`)**: Hilos independientes. Necesitan Loop infinito (`while(1)`).
2.  **Colas (`xQueueCreate`, `xQueueSend`, `xQueueReceive`)**:
    *   Pasan datos por **copia** (no referencia).
    *   Son "Thread safe" (seguras entre tareas).
    *   `portMAX_DELAY`: Espera infinita.
    *   `0`: No espera.
3.  **Semáforos (`xSemaphoreCreateBinary`, `xSemaphoreTake`, `xSemaphoreGive`)**:
    *   Sincronización (señalización).
    *   Binario: Como una bandera (0 o 1).
4.  **Desde ISR (`FromISR`)**:
    *   **NUNCA** usar funciones normales de FreeRTOS en una interrupción. Usar las terminadas en `FromISR`.
    *   Requieren manejo de `xHigherPriorityTaskWoken` para hacer el cambio de contexto rápido (`portYIELD_FROM_ISR`).
