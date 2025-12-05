// -*- mode: c++ -*-
// ----------------------------------------------------------
// Config.h - Fichero de configuración para Práctica 2
// ----------------------------------------------------------
// Autor: Josue Bellota Ichaso, Nuria Wagner Llorens
// Fecha: 12-5-2025
// ----------------------------------------------------------

#ifndef CONFIG_H_INCLUIDO
#define CONFIG_H_INCLUIDO
// ----------------------------------------------------------

#include <M5Stack.h>
#include "driver/uart.h"
#include "driver/timer.h"

// ----------------------------------------------------------
// ------------------- Definiciones de Hardware y Muestreo --
// ----------------------------------------------------------
#define PIN_SENSOR_GAS 36          // Pin analógico conectado al sensor MQ-2
#define BAUDIOS_SERIE 115200       // Velocidad del puerto serie

// --- Configuración del Timer ---
#define TIMER_DIVIDER 80           // Divisor para conseguir 1MHz de frecuencia base (80MHz/80)
#define TIMER_SCALE   (TIMER_BASE_CLK / TIMER_DIVIDER) // Escala del temporizador en Hz
#define TIMER_INTERVAL_S 0.01      // Intervalo de 10ms
#define ALARM_VALUE   (TIMER_INTERVAL_S * TIMER_SCALE) // Valor de la alarma

// --- Configuración del buffer de muestras ---
#define N_MUESTRAS_PROCESO 200     // Número total de muestras para cálculo
#define N_MUESTRAS_LCD 100         // Muestras entre actualizaciones LCD

// --- UART ---
#define UART_PORT          UART_NUM_0
#define UART_RX_PIN        3
#define UART_TX_PIN        1
#define UART_BUFFER_SIZE   1024


// ----------------------------------------------------------
// ------------------- Estados y Tipos de Datos -------------
// ----------------------------------------------------------

// Enumerador para el estado principal del sistema
typedef enum {
  STATE_CALIBRATION,
  STATE_RUNNING
} SystemState_t;

// Enumerador para los tipos de gas (reemplaza los #define)
typedef enum {
  GAS_BUTANO,
  GAS_METANO,
  GAS_HIDROGENO,
  GAS_LPG,
  GAS_ALCOHOL,
  GAS_HUMO
} GasType_t;

// Estructuras de datos (sin cambios respecto a Práctica 1)
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
  GasType_t gas_detectado; // Usando el enum
  float ppm;
  String nombre_gas;
} DatosGas_t;


// ----------------------------------------------------------
// ------------------- Variables Globales (extern) ----------
// ----------------------------------------------------------

// --- Flags para comunicación entre ISR y loop ---
extern volatile bool flag_timer;
extern volatile bool flag_uart;
extern volatile bool flag_lcd;
extern volatile char char_recibido;

// --- Estado del sistema ---
extern SystemState_t systemState;

// --- Buffer de muestras ---
extern int16_t bufferMuestras[N_MUESTRAS_PROCESO];
extern uint16_t indiceMuestra;
extern bool bufferLleno;
extern uint16_t contadorLCD;

// --- Datos del sensor ---
extern float K;
extern float R0;
extern DatosRaw_t datosRaw;
extern DatosVoltios_t datosVolt;
extern DatosGas_t datosGas;

// --- Constantes del sensor ---
#define VCC 5.0
#define RL 10.0

// ----------------------------------------------------------
// ------------------- Prototipos de Funciones --------------
// ----------------------------------------------------------

// --- Funciones de Configuración ---
void setup_timer();
void setup_uart();

// --- Rutinas de Interrupción (ISRs) ---
void IRAM_ATTR isr_timer(void *para);
void isr_uart();

// --- Lógica Principal ---
void calibrarSensor();
void capturarMuestra();
void procesarMuestras();
void calcularGasYPPM();

// --- Presentación de Datos ---
void mostrarPorLCD();
void mostrarPorSerie();
void enviarStringUART(const char* str);

// --- Utilidades ---
float convertirAVoltios(int lecturaADC);
float calcularRs(float voltaje);
float calcularPPM(float rs_ro_ratio, GasType_t gas);
String obtenerNombreGas(GasType_t gas);
float calcularR0DesdeVoltaje(float voltaje);


#endif // CONFIG_H_INCLUIDO
