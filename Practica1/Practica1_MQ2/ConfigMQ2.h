// -*- mode: c++ -*-
// ----------------------------------------------------------
// ConfigMQ2.h - ACTUALIZADO
// ----------------------------------------------------------
// Autor: Josue Bellota
// Fecha: 11-1-2025
// ----------------------------------------------------------

#ifndef CONFIG_MQ2_H_INCLUIDO
#define CONFIG_MQ2_H_INCLUIDO
// ----------------------------------------------------------

//#include <Arduino.h>
#include <M5Stack.h>

// ----------------------------------------------------------
// ------------------- Definiciones generales ----------------
// ----------------------------------------------------------
//#define DEBUG
#define PIN_SENSOR_GAS 36          // Pin analógico conectado al sensor MQ-2
#define N_MUESTRAS_PROCESO 200     // Número total de muestras para cálculo
#define N_MUESTRAS_LCD 100         // Muestras entre actualizaciones LCD
#define FRECUENCIA_MUESTREO 100    // Frecuencia de muestreo (Hz)
#define PERIODO_MUESTREO_MS 10     // Periodo de muestreo (ms)
#define BAUDIOS_SERIE 115200       // Velocidad del puerto serie

// Constantes para cálculo de gases MQ-2
#define VCC 5.0                    // Tensión de alimentación del sensor
#define RL 10.0                    // Resistencia de carga en kΩ

// Parámetros para cálculo de PPM (según datasheet MQ-2)
#define GAS_BUTANO 0
#define GAS_METANO 1
#define GAS_HIDROGENO 2
#define GAS_LPG 3
#define GAS_ALCOHOL 4
#define GAS_HUMO 5

// ----------------------------------------------------------
// ------------------- Tipos de datos ------------------------
// ----------------------------------------------------------

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
  float rs_ro_ratio;      // Relación Rs/R0
  int gas_detectado;      // Tipo de gas detectado
  float ppm;              // Concentración en PPM
  String nombre_gas;      // Nombre del gas
} DatosGas_t;

// ----------------------------------------------------------
// ------------------- Variables globales --------------------
// ----------------------------------------------------------

extern float K;         // Factor de conversión ADC → Voltios
extern float R0;        // Resistencia de referencia en aire limpio
extern DatosRaw_t datosRaw;
extern DatosVoltios_t datosVolt;
extern DatosGas_t datosGas;
extern bool bufferLleno;

// ----------------------------------------------------------
// ------------------- Prototipos de funciones ---------------
// ----------------------------------------------------------

// Calibraciones
void calibrarSensorReal();
void calibrarSensorSimulado();

// Adquisición y procesamiento
void capturarMuestra();
void procesarMuestras();
void calcularGasYPPM();

// Presentación de datos
void mostrarPorLCD();
void mostrarPorSerie();

// Utilidades
float convertirAVoltios(int lecturaADC);
float calcularRs(float voltaje);
float calcularPPM(float rs_ro_ratio, int gas);
String obtenerNombreGas(int gas);

// En la sección de prototipos, agregar:
float calcularR0DesdeVoltaje(float voltaje);

// ----------------------------------------------------------
#endif
// ----------------------------------------------------------