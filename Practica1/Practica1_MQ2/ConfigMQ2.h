// -*- mode: c++ -*-
//
// ----------------------------------------------------------
// ConfigMQ2.h
// ----------------------------------------------------------
// Autor: Josue Bellota
// Fecha: 11-1-2025
// ----------------------------------------------------------
// Descripción general:
// Este archivo define constantes, tipos de datos y prototipos
// para el sistema de medición con el sensor MQ-2 conectado
// al M5Stack Core (ESP32).
// ----------------------------------------------------------
// Incluye parámetros de calibración, estructura de datos,
// defines de hardware, y funciones auxiliares reutilizables.
// ----------------------------------------------------------

#ifndef CONFIG_MQ2_H_INCLUIDO
#define CONFIG_MQ2_H_INCLUIDO
// ----------------------------------------------------------

#include <Arduino.h>
#include <M5Stack.h>

// ----------------------------------------------------------
// ------------------- Definiciones generales ----------------
// ----------------------------------------------------------

#define PIN_SENSOR_GAS 36          // Pin analógico conectado al sensor MQ-2
#define N_MUESTRAS_PROCESO 200     // Número total de muestras para cálculo
#define N_MUESTRAS_LCD 100         // Muestras entre actualizaciones LCD
#define FRECUENCIA_MUESTREO 100    // Frecuencia de muestreo (Hz)
#define PERIODO_MUESTREO_MS 10     // Periodo de muestreo (ms)
#define BAUDIOS_SERIE 115200       // Velocidad del puerto serie

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

// ----------------------------------------------------------
// ------------------- Variables globales --------------------
// ----------------------------------------------------------

extern float K;         // Factor de conversión ADC → Voltios
extern float R0;        // Resistencia de referencia en aire limpio
extern DatosRaw_t datosRaw;
extern DatosVoltios_t datosVolt;
// En la sección de prototipos, agregar:
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

// Presentación de datos
void mostrarPorLCD();
void mostrarPorSerie();

// Utilidades
float convertirAVoltios(int lecturaADC);
float calcularR0(float voltaje);

// ----------------------------------------------------------
#endif
// ----------------------------------------------------------
