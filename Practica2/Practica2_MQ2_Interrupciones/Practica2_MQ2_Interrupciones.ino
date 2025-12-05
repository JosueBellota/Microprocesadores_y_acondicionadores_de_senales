// -*- mode: c++ -*-
// --------------------------------------------------------------------
// Practica2_MQ2_Interrupciones.ino
//
// Implementaci�n de un sistema de adquisici�n de datos para un sensor
// MQ-2 utilizando una arquitectura Round-Robin con interrupciones
// de Timer y UART gestionadas por ESP-IDF.
// --------------------------------------------------------------------
// Autor: Josue Bellota Ichaso, Nuria Wagner Llorens
// Fecha: 12-5-2025
// --------------------------------------------------------------------

#include "Config.h"

// ----------------------------------------------------------
// ------------------- Variables Globales --------------------
// ----------------------------------------------------------

// --- Flags para comunicaci�n entre ISR y loop ---
volatile bool flag_timer = false;
volatile bool flag_uart = false;
volatile bool flag_lcd = false;
volatile char char_recibido = 0;

// --- Estado del sistema ---
SystemState_t systemState = STATE_CALIBRATION;

// --- Buffer de muestras y contadores ---
int16_t bufferMuestras[N_MUESTRAS_PROCESO];
uint16_t indiceMuestra = 0;
bool bufferLleno = false;
uint16_t contadorLCD = 0;

// --- Datos del sensor y calibraci�n ---
float K = 0.0f;
float R0 = 0.0f;
DatosRaw_t datosRaw;
DatosVoltios_t datosVolt;
DatosGas_t datosGas;

// --- Task Handle para el evento de UART ---
static QueueHandle_t uart_event_queue;

// ----------------------------------------------------------
// ------------------- Setup --------------------------------
// ----------------------------------------------------------

void setup() {
  M5.begin();
  M5.Lcd.clear(BLACK);
  M5.Lcd.setTextColor(GREEN);
  M5.Lcd.setTextSize(2);
  M5.Lcd.setCursor(0, 0);
  
  // 1. Iniciar Serial de Arduino S�LO para calibraci�n
  Serial.begin(BAUDIOS_SERIE);
  delay(1000);
  
  M5.Lcd.println("Iniciando Calibracion...");
  calibrarSensor();
  
  // 2. Transici�n al estado de operaci�n
  systemState = STATE_RUNNING;
  M5.Lcd.clear(BLACK);
  M5.Lcd.setCursor(0, 0);
  M5.Lcd.println("Calibracion Finalizada.");
  M5.Lcd.println("Iniciando mediciones...");
  delay(2000);

  // 3. Finalizar Serial de Arduino para liberar UART0
  Serial.end();

  // 4. Configurar UART y Timer usando ESP-IDF
  setup_uart();
  setup_timer();
}

// ----------------------------------------------------------
// ------------------- Loop Principal -----------------------
// ----------------------------------------------------------

void loop() {
  switch (systemState) {
    case STATE_CALIBRATION:
      // El setup se encarga de esto, el loop no hace nada aqu�.
      break;

    case STATE_RUNNING:
      // --- Tarea controlada por el Timer ---
      if (flag_timer) {
        flag_timer = false; // Desactivar el flag lo antes posible
        
        procesarMuestras();
        calcularGasYPPM();

        if (++contadorLCD >= N_MUESTRAS_LCD) {
          flag_lcd = true; // Activar flag para el LCD
          contadorLCD = 0;
        }
      }

      // --- Tarea de actualizaci�n del LCD ---
      if (flag_lcd) {
        flag_lcd = false; // Desactivar flag
        mostrarPorLCD();
      }

      // --- Tarea controlada por la UART ---
      if (flag_uart) {
        flag_uart = false; // Desactivar flag
        if (char_recibido == 'a' || char_recibido == 'A') {
          mostrarPorSerie();
        }
      }
      break;
  }
}

// ----------------------------------------------------------
// ---------- Configuraci�n de Perif�ricos (ESP-IDF) --------
// ----------------------------------------------------------

void setup_timer() {
  timer_config_t config = {
    .alarm_en = TIMER_ALARM_EN,
    .counter_en = TIMER_PAUSE,
    .intr_type = TIMER_INTR_LEVEL,
    .counter_dir = TIMER_COUNT_UP,
    .auto_reload = TIMER_AUTORELOAD_EN,
    .divider = TIMER_DIVIDER
  };
  
  timer_init(TIMER_GROUP_0, TIMER_0, &config);
  timer_set_counter_value(TIMER_GROUP_0, TIMER_0, 0);
  timer_set_alarm_value(TIMER_GROUP_0, TIMER_0, ALARM_VALUE);
  timer_enable_intr(TIMER_GROUP_0, TIMER_0);
  timer_isr_register(TIMER_GROUP_0, TIMER_0, isr_timer, NULL, ESP_INTR_FLAG_IRAM, NULL);
  timer_start(TIMER_GROUP_0, TIMER_0);
}

void setup_uart() {
  uart_config_t uart_config = {
    .baud_rate = BAUDIOS_SERIE,
    .data_bits = UART_DATA_8_BITS,
    .parity = UART_PARITY_DISABLE,
    .stop_bits = UART_STOP_BITS_1,
    .flow_ctrl = UART_HW_FLOWCTRL_DISABLE
  };

  uart_param_config(UART_PORT, &uart_config);
  uart_set_pin(UART_PORT, UART_TX_PIN, UART_RX_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
  uart_driver_install(UART_PORT, UART_BUFFER_SIZE, UART_BUFFER_SIZE, 10, &uart_event_queue, 0);

  // Crear una tarea para manejar los eventos de UART
  xTaskCreate(isr_uart, "uart_event_task", 2048, NULL, 12, NULL);
}


// ----------------------------------------------------------
// ------------------- Rutinas de Interrupcion --------------
// ----------------------------------------------------------

void IRAM_ATTR isr_timer(void *para) {
  TIMERG0.int_clr_timers.t0 = 1;
  TIMERG0.hw_timer[0].config.alarm_en = 1;
  
  // Tarea cr�tica: capturar la muestra
  capturarMuestra();

  // Activar el flag para el loop principal
  flag_timer = true;
}

// Tarea que gestiona los eventos de la UART (recomendado por ESP-IDF)
void isr_uart(void *pvParameters) {
  uart_event_t event;
  uint8_t data[128];
  
  for(;;) {
    if(xQueueReceive(uart_event_queue, (void * )&event, (portTickType)portMAX_DELAY)) {
      switch(event.type) {
        case UART_DATA:
          if (event.size > 0) {
            uart_read_bytes(UART_PORT, &data, event.size, portMAX_DELAY);
            char_recibido = data[0]; // Solo nos interesa el primer caracter
            flag_uart = true;
          }
          break;
        default:
          break;
      }
    }
  }
  vTaskDelete(NULL);
}


// ----------------------------------------------------------
// ------------------- Logica de la Aplicacion --------------
// ----------------------------------------------------------

void calibrarSensor() {
  Serial.println(">>> Calibrando sensor... Mantenga en aire limpio.");
  delay(2000);

  float voltajeConocido = 1.5;
  int sumaADC = 0;
  for (int i = 0; i < 100; i++) {
    sumaADC += analogRead(PIN_SENSOR_GAS);
    delay(20);
  }
  int V2 = sumaADC / 100;

  K = voltajeConocido / V2;
  float Vout_aire = convertirAVoltios(V2);
  R0 = calcularR0DesdeVoltaje(Vout_aire);

  Serial.println(">>> Calibracion finalizada");
  Serial.print("K = "); Serial.println(K, 6);
  Serial.print("R0 = "); Serial.println(R0, 2);
}

void capturarMuestra() {
  bufferMuestras[indiceMuestra] = analogRead(PIN_SENSOR_GAS);
  
  indiceMuestra++;
  if (indiceMuestra >= N_MUESTRAS_PROCESO) {
    indiceMuestra = 0;
    bufferLleno = true;
  }
}

void procesarMuestras() {
  if (!bufferLleno) return;

  int16_t maximo = 0, minimo = 4095;
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
  datosVolt.media = convertirAVoltios(datosRaw.media);
}

void calcularGasYPPM() {
  if (datosVolt.media <= 0 || R0 <= 0) return;

  float Rs = calcularRs(datosVolt.media);
  datosGas.rs_ro_ratio = Rs / R0;

  if (datosGas.rs_ro_ratio < 1.0) datosGas.gas_detectado = GAS_HIDROGENO;
  else if (datosGas.rs_ro_ratio < 2.0) datosGas.gas_detectado = GAS_BUTANO;
  else if (datosGas.rs_ro_ratio < 3.0) datosGas.gas_detectado = GAS_LPG;
  else if (datosGas.rs_ro_ratio < 4.0) datosGas.gas_detectado = GAS_METANO;
  else if (datosGas.rs_ro_ratio < 5.0) datosGas.gas_detectado = GAS_ALCOHOL;
  else datosGas.gas_detectado = GAS_HUMO;
  
  datosGas.ppm = calcularPPM(datosGas.rs_ro_ratio, datosGas.gas_detectado);
  datosGas.nombre_gas = obtenerNombreGas(datosGas.gas_detectado);
}


// ----------------------------------------------------------
// ------------------- Funciones de Presentacion ------------
// ----------------------------------------------------------

void mostrarPorLCD() {
  M5.Lcd.fillRect(0, 60, 320, 180, BLACK);
  M5.Lcd.setCursor(0, 60);
  
  M5.Lcd.setTextColor(CYAN);
  M5.Lcd.printf("Media RAW: %d\n", datosRaw.media);
  
  M5.Lcd.setTextColor(YELLOW);
  M5.Lcd.printf("Media Volt: %.3fV\n\n", datosVolt.media);
  
  M5.Lcd.setTextColor(GREEN);
  M5.Lcd.setTextSize(3);
  M5.Lcd.printf("Gas: %s\n", datosGas.nombre_gas.c_str());
  M5.Lcd.printf("PPM: %.1f\n", datosGas.ppm);
  M5.Lcd.setTextSize(2);
}

void mostrarPorSerie() {
    char buffer[80];
    
    enviarStringUART("\r\n================ DATOS SENSOR ================\r\n");
    
    sprintf(buffer, "Media RAW: %d | Media Volt: %.3fV\r\n", datosRaw.media, datosVolt.media);
    enviarStringUART(buffer);

    sprintf(buffer, "Ratio Rs/R0: %.2f\r\n", datosGas.rs_ro_ratio);
    enviarStringUART(buffer);
    
    sprintf(buffer, "Gas Detectado: %s | PPM: %.1f\r\n", datosGas.nombre_gas.c_str(), datosGas.ppm);
    enviarStringUART(buffer);

    sprintf(buffer, "Calibracion: K=%.6f, R0=%.2f\r\n", K, R0);
    enviarStringUART(buffer);

    enviarStringUART("============================================\r\n");
}

void enviarStringUART(const char* str) {
  uart_write_bytes(UART_PORT, str, strlen(str));
}

// ----------------------------------------------------------
// ------------------- Funciones de Utilidad ----------------
// ----------------------------------------------------------

float convertirAVoltios(int lecturaADC) {
  return K * lecturaADC;
}

float calcularR0DesdeVoltaje(float voltaje) {
  float Rs_aire = (VCC - voltaje) * RL / voltaje;
  return Rs_aire / 9.8; // Valor t�pico para MQ-2 en aire limpio
}

float calcularRs(float voltaje) {
  if (voltaje <= 0) return RL * 1000; // Evitar divisi�n por cero
  return RL * (VCC - voltaje) / voltaje;
}

float calcularPPM(float rs_ro_ratio, GasType_t gas) {
  if (rs_ro_ratio <= 0) return 0;
  
  float a, b;
  switch(gas) {
    case GAS_BUTANO:    a = -0.48; b = 1.33; break;
    case GAS_METANO:    a = -0.38; b = 1.13; break;
    case GAS_HIDROGENO: a = -0.46; b = 1.25; break;
    case GAS_LPG:       a = -0.45; b = 1.20; break;
    case GAS_ALCOHOL:   a = -0.44; b = 1.18; break;
    case GAS_HUMO:
    default:            a = -0.42; b = 1.08; break;
  }
  
  return pow(10, a * log10(rs_ro_ratio) + b);
}

String obtenerNombreGas(GasType_t gas) {
  switch(gas) {
    case GAS_BUTANO:    return "BUTANO";
    case GAS_METANO:    return "METANO";
    case GAS_HIDROGENO: return "HIDROGENO";
    case GAS_LPG:       return "LPG";
    case GAS_ALCOHOL:   return "ALCOHOL";
    case GAS_HUMO:
    default:            return "HUMO";
  }
}
