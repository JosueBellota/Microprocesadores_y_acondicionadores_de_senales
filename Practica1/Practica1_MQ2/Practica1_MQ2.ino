// -*- mode: c++ -*-
// ----------------------------------------------------------
// Practica1_MQ2.ino - CORREGIDO PROBLEMAS MATEMÁTICOS
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
    // Calcular gas y PPM después de procesar muestras
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
  // Supuestos simulados más realistas
  float V1 = 1.5;        // Voltaje medido (ficticio)
  int V2 = 1200;         // Lectura ADC más realista

  K = V1 / V2;           // Relación voltios/valor digital
  R0 = 9.8;              // Valor simulado típico de resistencia base (kΩ)

  Serial.println(">>> Calibracion simulada completada");
  Serial.print("K = "); Serial.println(K, 6);
  Serial.print("R0 = "); Serial.println(R0, 2);
  M5.Lcd.println("Calibracion simulada OK");
}

// ----------------------------------------------------------
// calibrarSensorReal() - CORREGIDO
// ----------------------------------------------------------

void calibrarSensorReal() {
  M5.Lcd.println("Calibrando sensor real...");
  delay(2000);  // Más tiempo para estabilización

  Serial.println(">>> Iniciando calibracion real");
  
  // CORREGIDO: Para calibración, usar voltaje conocido (1.5V con potenciómetro)
  float voltajeConocido = 1.5;  // Voltaje medido con voltímetro

  // Lectura del ADC (promedio de varias lecturas con estabilización)
  int sumaADC = 0;
  int lecturasValidas = 0;
  
  for (int i = 0; i < 100; i++) {
    int lectura = analogRead(PIN_SENSOR_GAS);
    if (lectura > 0 && lectura < 4095) {  // Filtrar lecturas inválidas
      sumaADC += lectura;
      lecturasValidas++;
    }
    delay(20);
  }
  
  if (lecturasValidas == 0) {
    Serial.println("ERROR: No hay lecturas validas del ADC");
    M5.Lcd.println("ERROR CALIBRACION");
    return;
  }
  
  int V2 = sumaADC / lecturasValidas;

  // CORREGIDO: Calcular K correctamente
  K = voltajeConocido / V2;

  Serial.print("Lectura ADC promedio: "); Serial.println(V2);
  Serial.print("Factor K calculado: "); Serial.println(K, 6);

  // CORREGIDO: Calcular R0 usando la fórmula correcta para MQ-2 en aire limpio
  // En aire limpio, asumimos una relación Rs/R0 ≈ 9.8 (valor típico)
  // Para obtener R0, necesitamos medir Rs en aire limpio
  float Vout = convertirAVoltios(V2);
  R0 = calcularR0DesdeVoltaje(Vout);

  Serial.println(">>> Calibracion real completada");
  Serial.print("K = "); Serial.println(K, 6);
  Serial.print("R0 = "); Serial.println(R0, 2);
  Serial.print("Vout en aire limpio: "); Serial.println(Vout, 3);
  M5.Lcd.println("Calibracion real OK");
}

// ----------------------------------------------------------
// calcularR0DesdeVoltaje() - NUEVA FUNCIÓN PARA CALIBRACIÓN
// ----------------------------------------------------------
// Calcula R0 a partir del voltaje en aire limpio
// Fórmula: R0 = Rs / (Rs/R0_aire) donde Rs/R0_aire ≈ 9.8 para aire limpio
// ----------------------------------------------------------

float calcularR0DesdeVoltaje(float voltaje) {
  if (voltaje <= 0 || voltaje >= VCC) return 10.0; // Valor por defecto seguro
  
  // Calcular Rs del sensor en aire limpio
  float Rs_aire = (VCC - voltaje) * RL / voltaje;
  
  // En aire limpio, Rs/R0 ≈ 9.8 (valor típico para MQ-2)
  // Por lo tanto: R0 = Rs_aire / 9.8
  return Rs_aire / 9.8;
}

// ----------------------------------------------------------
// capturarMuestra()
// ----------------------------------------------------------

void capturarMuestra() {
  int lectura = analogRead(PIN_SENSOR_GAS);
  // Filtrar lecturas inválidas
  if (lectura < 0) lectura = 0;
  if (lectura > 4095) lectura = 4095;
  
  bufferMuestras[indiceMuestra] = lectura;
  
  indiceMuestra++;
  if (indiceMuestra >= N_MUESTRAS_PROCESO) {
    indiceMuestra = 0;
    bufferLleno = true;  // Marcar que el buffer está lleno
  }
}

// ----------------------------------------------------------
// procesarMuestras() - CON PROTECCIÓN CONTRA VALORES INVÁLIDOS
// ----------------------------------------------------------

void procesarMuestras() {
  if (!bufferLleno) return;
  
  int16_t maximo = bufferMuestras[0];
  int16_t minimo = bufferMuestras[0];
  int32_t suma = 0;
  int muestrasValidas = 0;

  for (int i = 0; i < N_MUESTRAS_PROCESO; i++) {
    int16_t valor = bufferMuestras[i];
    if (valor >= 0 && valor <= 4095) {  // Solo procesar lecturas válidas
      if (valor > maximo) maximo = valor;
      if (valor < minimo) minimo = valor;
      suma += valor;
      muestrasValidas++;
    }
  }

  if (muestrasValidas == 0) {
    // Si no hay muestras válidas, usar valores por defecto
    datosRaw.maximo = 0;
    datosRaw.minimo = 0;
    datosRaw.media = 0;
    datosVolt.maximo = 0;
    datosVolt.minimo = 0;
    datosVolt.media = 0;
    return;
  }

  datosRaw.maximo = maximo;
  datosRaw.minimo = minimo;
  datosRaw.media = suma / muestrasValidas;

  // CORREGIDO: Verificar que K sea válido antes de convertir
  if (K > 0.000001 && K < 1.0) {
    datosVolt.maximo = convertirAVoltios(maximo);
    datosVolt.minimo = convertirAVoltios(minimo);
    datosVolt.media = convertirAVoltios(datosRaw.media);
  } else {
    // Si K no es válido, usar valores por defecto
    datosVolt.maximo = 0;
    datosVolt.minimo = 0;
    datosVolt.media = 0;
    Serial.println("ADVERTENCIA: Factor K invalido, revisar calibracion");
  }
}

// ----------------------------------------------------------
// calcularGasYPPM() - CORREGIDO CON PROTECCIONES
// ----------------------------------------------------------

void calcularGasYPPM() {
  // Verificar que los datos sean válidos
  if (datosVolt.media <= 0 || datosVolt.media >= VCC || R0 <= 0) {
    datosGas.rs_ro_ratio = 10.0;  // Valor por defecto seguro
    datosGas.gas_detectado = GAS_HUMO;
    datosGas.ppm = 0;
    datosGas.nombre_gas = "SIN DATOS";
    return;
  }

  // Calcular Rs usando la tensión MEDIA
  float Rs = calcularRs(datosVolt.media);
  
  // Calcular relación Rs/R0
  if (R0 > 0) {
    datosGas.rs_ro_ratio = Rs / R0;
  } else {
    datosGas.rs_ro_ratio = 10.0;  // Valor por defecto
  }
  
  // Determinar el gas basado en la relación Rs/R0
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
// calcularRs() - CORREGIDA CON FÓRMULA CORRECTA
// ----------------------------------------------------------
// Fórmula correcta para divisor de voltaje:
// Vout = Vcc * (RL / (Rs + RL))
// Por lo tanto: Rs = RL * (Vcc - Vout) / Vout
// ----------------------------------------------------------

float calcularRs(float voltaje) {
  if (voltaje <= 0 || voltaje >= VCC) return R0 * 10.0; // Valor seguro
  
  // Fórmula corregida para el circuito MQ-2
  return RL * (VCC - voltaje) / voltaje;
}

// ----------------------------------------------------------
// calcularPPM() - CON PROTECCIÓN CONTRA VALORES INVÁLIDOS
// ----------------------------------------------------------

float calcularPPM(float rs_ro_ratio, int gas) {
  if (rs_ro_ratio <= 0 || isnan(rs_ro_ratio) || isinf(rs_ro_ratio)) {
    return 0;
  }
  
  // Parámetros aproximados basados en curvas del datasheet MQ-2
  float a, b;
  
  switch(gas) {
    case GAS_BUTANO:
      a = -0.48; b = 1.33;
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
  float log_ratio = log10(rs_ro_ratio);
  float ppm = pow(10, a * log_ratio + b);
  
  // Limitar valores extremos
  if (ppm < 0) ppm = 0;
  if (ppm > 10000) ppm = 10000;
  
  return ppm;
}

// ----------------------------------------------------------
// obtenerNombreGas()
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
// mostrarPorSerie() - CON INFORMACIÓN DE DIAGNÓSTICO
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
  Serial.println("Parametros de calibracion:");
  Serial.print("  K: "); Serial.println(K, 6);
  Serial.print("  R0: "); Serial.println(R0, 2);
  Serial.println("==================================================");
}

// ----------------------------------------------------------
// mostrarPorLCD() - CON PROTECCIÓN CONTRA VALORES INVÁLIDOS
// ----------------------------------------------------------

void mostrarPorLCD() {
  M5.Lcd.fillRect(0, 60, 320, 160, BLACK);
  
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
  
  // Mostrar valores con protección contra NaN/inf
  if (!isnan(datosVolt.maximo) && !isinf(datosVolt.maximo)) {
    M5.Lcd.printf("Max: %.3fV   ", datosVolt.maximo);
  } else {
    M5.Lcd.printf("Max: ---V   ");
  }
  
  if (!isnan(datosVolt.minimo) && !isinf(datosVolt.minimo)) {
    M5.Lcd.printf("Min: %.3fV\n", datosVolt.minimo);
  } else {
    M5.Lcd.printf("Min: ---V\n");
  }
  
  if (!isnan(datosVolt.media) && !isinf(datosVolt.media)) {
    M5.Lcd.printf("Media: %.3fV\n\n", datosVolt.media);
  } else {
    M5.Lcd.printf("Media: ---V\n\n");
  }
  
  M5.Lcd.setTextColor(GREEN);
  M5.Lcd.println("DETECCION GAS:");
  M5.Lcd.setTextColor(WHITE);
  
  if (!isnan(datosGas.rs_ro_ratio) && !isinf(datosGas.rs_ro_ratio)) {
    M5.Lcd.printf("Rs/R0: %.2f\n", datosGas.rs_ro_ratio);
  } else {
    M5.Lcd.printf("Rs/R0: ---\n");
  }
  
  M5.Lcd.printf("Gas: %s\n", datosGas.nombre_gas.c_str());
  
  if (!isnan(datosGas.ppm) && !isinf(datosGas.ppm)) {
    M5.Lcd.printf("PPM: %.1f\n", datosGas.ppm);
  } else {
    M5.Lcd.printf("PPM: ---\n");
  }
  
  // Información adicional
  M5.Lcd.setTextColor(GREEN);
  M5.Lcd.setTextSize(1);
  M5.Lcd.setCursor(0, 220);
  M5.Lcd.println("Presiona 'a' en Monitor Serie para ver datos");
}

// ----------------------------------------------------------
// convertirAVoltios() - CON PROTECCIÓN
// ----------------------------------------------------------

float convertirAVoltios(int lecturaADC) {
  if (lecturaADC < 0 || lecturaADC > 4095) return 0;
  if (K <= 0 || K > 1.0) return 0;  // K debe ser un valor razonable
  return K * lecturaADC;
}