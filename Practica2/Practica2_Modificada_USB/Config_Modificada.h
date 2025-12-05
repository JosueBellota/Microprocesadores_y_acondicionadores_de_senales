// -*- mode: c++ -*-
// ----------------------------------------------------------
// Config_Modificada.h - Configuración para Práctica 2
// ----------------------------------------------------------
// Autor: Josue Bellota Ichaso, Nuria Wagner Llorens
// Fecha: 12-5-2025
// ----------------------------------------------------------

#ifndef CONFIG_MODIFICADA_H_INCLUIDO
#define CONFIG_MODIFICADA_H_INCLUIDO

#include <M5Stack.h>
#include "driver/timer.h"
#include "driver/uart.h"

// ----------------------------------------------------------
// ------------------- Definiciones de Hardware -------------
// ----------------------------------------------------------
#define PIN_SENSOR_GAS 36          // Pin analógico conectado al sensor MQ-2
#define BAUDIOS_SERIE 115200       // Velocidad del puerto serie

// --- Configuración del Timer ---
#define TIMER_DIVIDER 80           // Divisor para 1MHz (80MHz/80)
#define TIMER_SCALE (TIMER_BASE_CLK / TIMER_DIVIDER)
#define TIMER_INTERVAL_S 0.01      // 10ms (100Hz)
#define ALARM_VALUE (TIMER_INTERVAL_S * TIMER_SCALE)

// --- Configuración de la UART (UART0 - USB) ---
#define UART_NUM UART_NUM_0
#define UART_TXD_PIN 1
#define UART_RXD_PIN 3
#define UART_BUF_SIZE 1024         // Requisito obligatorio del usuario
#define UART_QUEUE_LEN 10

// --- Configuración del buffer de muestras ---
#define N_MUESTRAS_PROCESO 200     // Número total de muestras para cálculo
#define N_MUESTRAS_LCD 100         // Muestras entre actualizaciones LCD

// ----------------------------------------------------------
// ------------------- Estados y Tipos de Datos -------------
// ----------------------------------------------------------

// Enumerador para los tipos de gas
typedef enum {
  GAS_BUTANO,
  GAS_METANO,
  GAS_HIDROGENO,
  GAS_LPG,
  GAS_ALCOHOL,
  GAS_HUMO
} GasType_t;

// Estructuras de datos
typedef struct {
  int16_t maximo;
  int16_t minimo;
  int32_t media;
} DatosRaw_t;

typedef struct {
  float maximo;
  float minimo;
  float media;
} DatosVoltios_t;

typedef struct {
  float rs_ro_ratio;
  GasType_t gas_detectado;
  float ppm;
  String nombre_gas;
} DatosGas_t;

// ----------------------------------------------------------
// ------------------- Constantes del sensor ----------------
// ----------------------------------------------------------
#define VCC 5.0
#define RL 10.0

// ----------------------------------------------------------
// ------------------- Prototipos de Funciones --------------
// ----------------------------------------------------------

// --- Funciones de Configuración ---
void setup_timer();
void setup_uart();

// --- Tarea que gestiona eventos de UART (actúa como ISR) ---
void uart_event_task(void *pvParameters);

// --- Rutina de Interrupción de Timer ---
void IRAM_ATTR isr_timer(void *para);

// --- Lógica Principal ---
void calibrarSensor();
void capturarMuestra();
void procesarMuestras();
void calcularGasYPPM();

// --- Presentación de Datos ---
void mostrarPorLCD();
void mostrarPorSerie();

// --- Utilidades ---
float convertirAVoltios(int lecturaADC);
float calcularRs(float voltaje);
float calcularPPM(float rs_ro_ratio, GasType_t gas);
String obtenerNombreGas(GasType_t gas);
float calcularR0DesdeVoltaje(float voltaje);
void uart_send_float(float value, int decimals);
void uart_send_int(int value);
void uart_send_string(const char* str);

#endif // CONFIG_MODIFICADA_H_INCLUIDO