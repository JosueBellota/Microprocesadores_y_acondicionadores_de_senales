// -*- mode: c++ -*-
// ----------------------------------------------------------
// Practica1_MQ2.ino - ACTUALIZADO CON DETECCIÓN DE GAS
// ----------------------------------------------------------
// Autor: Josue Bellota Ichaso
// Fecha: [YYYY-MM-DD]
// ----------------------------------------------------------

#include "ConfigMQ2.h"

// ----------------------------------------------------------
// ------------------- Variables globales --------------------
// ----------------------------------------------------------

float K = 0.0f;            // Factor de conversión ADC → Voltios
float R0 = 0.0f;           // Resistencia de referencia en aire limpio
DatosRaw_t datosRaw;       // Datos en bruto
DatosVoltios_t datosVolt;  // Datos convertidos a voltios
DatosGas_t datosGas;       // Datos de gas detectado

// Buffer circular de muestras
int16_t bufferMuestras[N_MUESTRAS_PROCESO];
uint16_t indiceMuestra = 0;
bool bufferLleno = false;  // Indica cuando el buffer tiene 200 muestras

// Contador de muestras para actualización LCD
uint16_t contadorLCD = 0;

// ----------------------------------------------------------
// setup()
// ----------------------------------------------------------

void setup() {
  M5.begin();
  Serial.begin(BAUDIOS_SERIE);
  
  // Inicializar buffer
  for(int i = 0; i < N_MUESTRAS_PROCESO; i++) {
    bufferMuestras[i] = 0;
  }
  
  M5.Lcd.clear(BLACK);
  M5.Lcd.setTextColor(GREEN);
  M5.Lcd.setTextSize(2);
  M5.Lcd.setCursor(0, 0);
  M5.Lcd.println("PRACTICA 1 MQ-2");
  M5.Lcd.println("------------------");
  delay(1000);

  calibrarSensorReal();   // Usar esta para modo real
  // calibrarSensorSimulado(); // Usar esta para pruebas
}

// ----------------------------------------------------------
// loop()
// ----------------------------------------------------------

void loop() {
  unsigned long inicioLoop = millis();
  
  // 1. Captura de muestra analógica
  capturarMuestra();

  // 2. Procesamiento cada 200 muestras (cuando el buffer está lleno)
  if (bufferLleno) {
    procesarMuestras();
    // NUEVO: Calcular gas y PPM después de procesar muestras
    calcularGasYPPM();
  }

  // 3. Si se reciben datos por el puerto serie
  if (Serial.available()) {
    char c = Serial.read();
    if (c == 'a' || c == 'A') {
      mostrarPorSerie();
    }
  }

  // 4. Actualización LCD cada 100 muestras
  if (++contadorLCD >= N_MUESTRAS_LCD) {
    mostrarPorLCD();
    contadorLCD = 0;
  }

  // 5. Retardo para mantener periodo de muestreo
  unsigned long tiempoTranscurrido = millis() - inicioLoop;
  if (tiempoTranscurrido < PERIODO_MUESTREO_MS) {
    delay(PERIODO_MUESTREO_MS - tiempoTranscurrido);
  }
}

// ----------------------------------------------------------
// calibrarSensorSimulado()
// ----------------------------------------------------------

void calibrarSensorSimulado() {
  // Supuestos simulados
  float V1 = 1.5;   // Voltaje medido (ficticio)
  int V2 = 1800;    // Lectura ADC (ficticia)

  K = V1 / V2;      // Relación voltios/valor digital
  R0 = 10.0;        // Valor simulado de resistencia base (kΩ)

  Serial.println(">>> Calibracion simulada completada");
  Serial.print("K = "); Serial.println(K, 6);
  Serial.print("R0 = "); Serial.println(R0, 2);
  M5.Lcd.println("Calibracion simulada OK");
}

// ----------------------------------------------------------
// calibrarSensorReal()
// ----------------------------------------------------------

void calibrarSensorReal() {
  M5.Lcd.println("Calibrando sensor real...");
  delay(1000);

  // Valor aplicado con potenciómetro (~1.5V medido con voltímetro)
  float V1 = 1.5;

  // Lectura del ADC (promedio de varias lecturas)
  int sumaADC = 0;
  for (int i = 0; i < 50; i++) {
    sumaADC += analogRead(PIN_SENSOR_GAS);
    delay(10);
  }
  int V2 = sumaADC / 50;

  K = V1 / V2;

  // Medida de resistencia en aire limpio (para MQ-2)
  float Vgas = convertirAVoltios(V2);
  R0 = calcularR0(Vgas);

  Serial.println(">>> Calibracion real completada");
  Serial.print("K = "); Serial.println(K, 6);
  Serial.print("R0 = "); Serial.println(R0, 2);
  M5.Lcd.println("Calibracion real OK");
}

// ----------------------------------------------------------
// capturarMuestra()
// ----------------------------------------------------------

void capturarMuestra() {
  int lectura = analogRead(PIN_SENSOR_GAS);
  bufferMuestras[indiceMuestra] = lectura;
  
  indiceMuestra++;
  if (indiceMuestra >= N_MUESTRAS_PROCESO) {
    indiceMuestra = 0;
    bufferLleno = true;  // Marcar que el buffer está lleno
  }
}

// ----------------------------------------------------------
// procesarMuestras()
// ----------------------------------------------------------

void procesarMuestras() {
  if (!bufferLleno) return;  // Solo procesar si el buffer está lleno
  
  int16_t maximo = bufferMuestras[0];
  int16_t minimo = bufferMuestras[0];
  int32_t suma = 0;

  for (int i = 0; i < N_MUESTRAS_PROCESO; i++) {
    int16_t valor = bufferMuestras[i];
    if (valor > maximo) maximo = valor;
    if (valor < minimo) minimo = valor;
    suma += valor;
  }

  datosRaw.maximo = maximo;
  datosRaw.minimo = minimo;
  datosRaw.media = suma / N_MUESTRAS_PROCESO;

  datosVolt.maximo = convertirAVoltios(maximo);
  datosVolt.minimo = convertirAVoltios(minimo);
  datosVolt.media  = convertirAVoltios(datosRaw.media);
}

// ----------------------------------------------------------
// calcularGasYPPM() - NUEVA FUNCIÓN
// ----------------------------------------------------------
// Calcula la relación Rs/R0 y determina el gas y PPM
// Basado en las fórmulas del profesor y datasheet MQ-2
// ----------------------------------------------------------

void calcularGasYPPM() {
  // Calcular Rs usando la tensión media
  float Rs = calcularRs(datosVolt.media);
  
  // Calcular relación Rs/R0 (fórmula del profesor)
  datosGas.rs_ro_ratio = Rs / R0;
  
  // Determinar el gas basado en la relación Rs/R0
  // Valores típicos del datasheet MQ-2:
  if (datosGas.rs_ro_ratio < 1.0) {
    datosGas.gas_detectado = GAS_HIDROGENO;
  } else if (datosGas.rs_ro_ratio < 2.0) {
    datosGas.gas_detectado = GAS_BUTANO;
  } else if (datosGas.rs_ro_ratio < 3.0) {
    datosGas.gas_detectado = GAS_LPG;
  } else if (datosGas.rs_ro_ratio < 4.0) {
    datosGas.gas_detectado = GAS_METANO;
  } else if (datosGas.rs_ro_ratio < 5.0) {
    datosGas.gas_detectado = GAS_ALCOHOL;
  } else {
    datosGas.gas_detectado = GAS_HUMO;
  }
  
  // Calcular PPM
  datosGas.ppm = calcularPPM(datosGas.rs_ro_ratio, datosGas.gas_detectado);
  datosGas.nombre_gas = obtenerNombreGas(datosGas.gas_detectado);
}

// ----------------------------------------------------------
// calcularRs() - NUEVA FUNCIÓN
// ----------------------------------------------------------
// Calcula Rs basado en la fórmula del profesor:
// Vanal = Vcc * (R0 / (R0 + Rs)) => Rs/R0 = (Vcc/Vanal) - 1
// ----------------------------------------------------------

float calcularRs(float voltaje) {
  // Aplicando la fórmula: Rs/R0 = (Vcc/Vanal) - 1
  // Por lo tanto: Rs = R0 * ((Vcc/Vanal) - 1)
  if (voltaje <= 0) return R0; // Evitar división por cero
  return R0 * ((VCC / voltaje) - 1);
}

// ----------------------------------------------------------
// calcularPPM() - NUEVA FUNCIÓN
// ----------------------------------------------------------
// Calcula PPM basado en la relación Rs/R0 y curvas del datasheet
// Usando aproximación logarítmica típica de sensores MQ
// ----------------------------------------------------------

float calcularPPM(float rs_ro_ratio, int gas) {
  // Parámetros aproximados basados en curvas del datasheet MQ-2
  // Estos valores deben calibrarse con gases de referencia
  float a, b;
  
  switch(gas) {
    case GAS_BUTANO:
      a = -0.48; b = 1.33;  // Parámetros aproximados
      break;
    case GAS_METANO:
      a = -0.38; b = 1.13;
      break;
    case GAS_HIDROGENO:
      a = -0.46; b = 1.25;
      break;
    case GAS_LPG:
      a = -0.45; b = 1.20;
      break;
    case GAS_ALCOHOL:
      a = -0.44; b = 1.18;
      break;
    case GAS_HUMO:
      a = -0.42; b = 1.08;
      break;
    default:
      a = -0.40; b = 1.10;
  }
  
  // Fórmula: log(PPM) = a * log(Rs/R0) + b
  // Por lo tanto: PPM = 10^(a * log(Rs/R0) + b)
  if (rs_ro_ratio <= 0) return 0;
  float log_ratio = log10(rs_ro_ratio);
  return pow(10, a * log_ratio + b);
}

// ----------------------------------------------------------
// obtenerNombreGas() - NUEVA FUNCIÓN
// ----------------------------------------------------------

String obtenerNombreGas(int gas) {
  switch(gas) {
    case GAS_BUTANO: return "BUTANO";
    case GAS_METANO: return "METANO";
    case GAS_HIDROGENO: return "HIDROGENO";
    case GAS_LPG: return "LPG";
    case GAS_ALCOHOL: return "ALCOHOL";
    case GAS_HUMO: return "HUMO";
    default: return "DESCONOCIDO";
  }
}

// ----------------------------------------------------------
// mostrarPorSerie() - ACTUALIZADO CON DATOS DE GAS
// ----------------------------------------------------------

void mostrarPorSerie() {
  Serial.println("==================================================");
  Serial.println("           DATOS DEL SENSOR MQ-2");
  Serial.println("==================================================");
  Serial.println("Datos en bruto (RAW):");
  Serial.print("  Maximo: "); Serial.println(datosRaw.maximo);
  Serial.print("  Minimo: "); Serial.println(datosRaw.minimo);
  Serial.print("  Media : "); Serial.println(datosRaw.media);
  Serial.println("Datos en voltios:");
  Serial.print("  Maximo: "); Serial.print(datosVolt.maximo, 3); Serial.println(" V");
  Serial.print("  Minimo: "); Serial.print(datosVolt.minimo, 3); Serial.println(" V");
  Serial.print("  Media : "); Serial.print(datosVolt.media, 3); Serial.println(" V");
  Serial.println("Datos de gas detectado:");
  Serial.print("  Rs/R0: "); Serial.println(datosGas.rs_ro_ratio, 3);
  Serial.print("  Gas: "); Serial.println(datosGas.nombre_gas);
  Serial.print("  PPM: "); Serial.println(datosGas.ppm, 1);
  Serial.println("==================================================");
}

// ----------------------------------------------------------
// mostrarPorLCD() - ACTUALIZADO CON DATOS DE GAS
// ----------------------------------------------------------

void mostrarPorLCD() {
  M5.Lcd.fillRect(0, 60, 320, 160, BLACK);  // Limpia área de datos
  
  M5.Lcd.setCursor(0, 60);
  M5.Lcd.setTextColor(CYAN);
  M5.Lcd.setTextSize(2);
  M5.Lcd.println("DATOS RAW:");
  M5.Lcd.setTextColor(WHITE);
  M5.Lcd.printf("Max: %d   Min: %d\n", datosRaw.maximo, datosRaw.minimo);
  M5.Lcd.printf("Media: %d\n\n", datosRaw.media);
  
  M5.Lcd.setTextColor(YELLOW);
  M5.Lcd.println("DATOS VOLTIOS:");
  M5.Lcd.setTextColor(WHITE);
  M5.Lcd.printf("Max: %.3fV   Min: %.3fV\n", datosVolt.maximo, datosVolt.minimo);
  M5.Lcd.printf("Media: %.3fV\n\n", datosVolt.media);
  
  M5.Lcd.setTextColor(GREEN);
  M5.Lcd.println("DETECCION GAS:");
  M5.Lcd.setTextColor(WHITE);
  M5.Lcd.printf("Rs/R0: %.2f\n", datosGas.rs_ro_ratio);
  M5.Lcd.printf("Gas: %s\n", datosGas.nombre_gas.c_str());
  M5.Lcd.printf("PPM: %.1f\n", datosGas.ppm);
  
  // Información adicional
  M5.Lcd.setTextColor(GREEN);
  M5.Lcd.setTextSize(1);
  M5.Lcd.setCursor(0, 220);
  M5.Lcd.println("Presiona 'a' en Monitor Serie para ver datos");
}

// ----------------------------------------------------------
// convertirAVoltios()
// ----------------------------------------------------------

float convertirAVoltios(int lecturaADC) {
  return K * lecturaADC;
}

// ----------------------------------------------------------
// calcularR0() - CORREGIDO (usa las constantes del .h)
// ----------------------------------------------------------

float calcularR0(float voltaje) {
  // CORRECCIÓN: Usar las constantes definidas en ConfigMQ2.h
  // VCC y RL ya están definidos, no redefinirlos aquí
  float Rs = (VCC - voltaje) * RL / voltaje;
  return Rs; // R0 en aire limpio
}