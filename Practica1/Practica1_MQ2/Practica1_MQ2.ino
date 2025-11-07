// -*- mode: c++ -*-
// ----------------------------------------------------------
// Practica1_MQ2.ino - CORREGIDO
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

  // SOLUCIÓN: Solo una calibración
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
// mostrarPorSerie() - MEJORADO
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
  Serial.println("==================================================");
}

// ----------------------------------------------------------
// mostrarPorLCD() - CORREGIDO (ahora muestra RAW y Voltios)
// ----------------------------------------------------------

void mostrarPorLCD() {
  M5.Lcd.fillRect(0, 60, 320, 140, BLACK);  // Limpia área de datos
  
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
  M5.Lcd.printf("Media: %.3fV\n", datosVolt.media);
  
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
// calcularR0()
// ----------------------------------------------------------

float calcularR0(float voltaje) {
  const float Vc = 5.0;     // Tensión de alimentación
  const float RL = 10.0;    // Resistencia de carga en kΩ
  float Rs = (Vc - voltaje) * RL / voltaje;
  return Rs; // R0 en aire limpio
}