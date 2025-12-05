// -*- mode: c++ -*-
// --------------------------------------------------------------------
// Practica2_Modificada_USB.ino
// 
// Implementación de la arquitectura SW Round-Robin con interrupciones
// Usa interrupciones de Timer (100Hz) y UART (ESP-IDF)
// La comunicación serie se hace a través del USB (UART0) con interrupciones
// --------------------------------------------------------------------
// Autor: Josue Bellota Ichaso, Nuria Wagner Llorens
// Fecha: 12-5-2025
// --------------------------------------------------------------------

#include "Config_Modificada.h"

// ----------------------------------------------------------
// ------------------- Variables Globales --------------------
// ----------------------------------------------------------

// --- Flags para comunicación entre ISR y loop ---
volatile bool flag_timer = false;
volatile bool flag_lcd = false;
volatile bool flag_uart = false;

// --- Buffer de muestras y contadores ---
int16_t bufferMuestras[N_MUESTRAS_PROCESO];
uint16_t indiceMuestra = 0;
bool bufferLleno = false;
uint16_t contadorLCD = 0;

// --- Variables para UART ---
char uart_received_char = 0;
QueueHandle_t uart_queue;

// --- Datos del sensor y calibración ---
float K = 0.0f;
float R0 = 0.0f;
DatosRaw_t datosRaw;
DatosVoltios_t datosVolt;
DatosGas_t datosGas;

// ----------------------------------------------------------
// ------------------- Setup --------------------------------
// ----------------------------------------------------------

void setup() {
  M5.begin();
  M5.Lcd.clear(BLACK);
  M5.Lcd.setTextColor(GREEN);
  M5.Lcd.setTextSize(2);
  M5.Lcd.setCursor(0, 0);
  
  // Iniciar Serial de Arduino solo para calibración
  Serial.begin(BAUDIOS_SERIE);
  delay(1000);
  
  M5.Lcd.println("Iniciando Calibracion...");
  calibrarSensor();
  
  M5.Lcd.clear(BLACK);
  M5.Lcd.setCursor(0, 0);
  M5.Lcd.println("Calibracion Finalizada.");
  M5.Lcd.println("Desactivando Serial...");
  delay(1000);
  
  // Desactivar Serial de Arduino (como indica la NOTA1)
  Serial.end();
  delay(100); // Pequeña pausa para asegurar que se libere el puerto
  
  // Configurar UART con ESP-IDF (interrupciones)
  setup_uart();
  
  M5.Lcd.println("Configurando Timer...");
  // Configurar Timer con ESP-IDF
  setup_timer();
  
  M5.Lcd.println("Sistema listo!");
  M5.Lcd.println("Envie 'a' por Serial");
  M5.Lcd.println("para ver datos");
  delay(2000);
  M5.Lcd.clear(BLACK);
}

// ----------------------------------------------------------
// ------------------- Loop Principal -----------------------
// ----------------------------------------------------------

void loop() {
  // --- Tarea controlada por el Timer ---
  if (flag_timer) {
    flag_timer = false; // Desactivar el flag
    
    // Solo procesar si el buffer está lleno
    if (bufferLleno) {
        procesarMuestras();
        calcularGasYPPM();
    }

    if (++contadorLCD >= N_MUESTRAS_LCD) {
      flag_lcd = true; // Activar flag para el LCD
      contadorLCD = 0;
    }
  }

  // --- Tarea de actualización del LCD ---
  if (flag_lcd) {
    flag_lcd = false; // Desactivar flag
    if (bufferLleno) { // Solo mostrar si ya hay datos válidos
        mostrarPorLCD();
    }
  }

  // --- Tarea controlada por la UART (con interrupciones) ---
  if (flag_uart) {
    flag_uart = false; // Desactivar flag
    
    if (uart_received_char == 'a' || uart_received_char == 'A') {
      if (bufferLleno) { // Solo mostrar si ya hay datos válidos
        mostrarPorSerie();
      }
    }
    uart_received_char = 0; // Limpiar carácter recibido
  }
}

// ----------------------------------------------------------
// ---------- Configuración de Periféricos (ESP-IDF) --------
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
  // 1. Configurar parámetros de la UART
  uart_config_t uart_config = {
    .baud_rate = BAUDIOS_SERIE,
    .data_bits = UART_DATA_8_BITS,
    .parity = UART_PARITY_DISABLE,
    .stop_bits = UART_STOP_BITS_1,
    .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
    .rx_flow_ctrl_thresh = 122,
    .source_clk = UART_SCLK_APB,
  };
  
  // Configurar parámetros UART
  uart_param_config(UART_NUM, &uart_config);
  
  // 2. Asignar pines
  uart_set_pin(UART_NUM, UART_TXD_PIN, UART_RXD_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
  
  // 3. Instalar driver con buffer y cola de eventos
  uart_driver_install(UART_NUM, UART_BUF_SIZE * 2, UART_BUF_SIZE * 2, UART_QUEUE_LEN, &uart_queue, 0);
  
  // 4. Configurar interrupción de recepción básica
  uart_enable_rx_intr(UART_NUM);
  
  // 5. Registrar subrutina de interrupción
  uart_isr_free(UART_NUM);
  uart_isr_register(UART_NUM, isr_uart, NULL, ESP_INTR_FLAG_IRAM, NULL);
}

// ----------------------------------------------------------
// ------------------- Rutinas de Interrupción --------------
// ----------------------------------------------------------

void IRAM_ATTR isr_timer(void *para) {
  // Reset de la alarma
  TIMERG0.int_clr_timers.t0 = 1;
  TIMERG0.hw_timer[0].config.alarm_en = 1;
  
  // Tarea crítica: capturar la muestra
  capturarMuestra();

  // Activar el flag para el loop principal
  flag_timer = true;
}

// ISR de UART simplificada y compatible
void IRAM_ATTR isr_uart(void *arg) {
  // Variables locales para la ISR
  uint8_t rx_data;
  uint32_t uart_status;
  
  // Obtener estado de UART
  uart_status = UART0.int_st.val;
  
  // Si hay datos en el buffer de recepción
  if (uart_status & UART_RXFIFO_FULL_INT_ST || uart_status & UART_RXFIFO_TOUT_INT_ST) {
    // Leer un carácter
    while (uart_read_bytes(UART_NUM, &rx_data, 1, 0) > 0) {
      uart_received_char = (char)rx_data;
      flag_uart = true;
    }
    
    // Limpiar flags de interrupción (forma alternativa compatible)
    UART0.int_clr.rxfifo_full = 1;
    UART0.int_clr.rxfifo_tout = 1;
  }
}

// ----------------------------------------------------------
// ------------------- Lógica de la Aplicación --------------
// ----------------------------------------------------------

void calibrarSensor() {
  // Usamos Serial de Arduino para calibración (NOTA1)
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
    if (!bufferLleno) {
        bufferLleno = true; // Marcar solo la primera vez que se llena
    }
  }
}

void procesarMuestras() {
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
// ------------------- Funciones de Presentación ------------
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
  // Enviar cabecera
  uart_send_string("\r\n================ DATOS SENSOR ================\r\n");
  
  // Media RAW
  uart_send_string("Media RAW: ");
  uart_send_int(datosRaw.media);
  
  // Media Volt
  uart_send_string(" | Media Volt: ");
  uart_send_float(datosVolt.media, 3);
  uart_send_string("V\r\n");
  
  // Ratio Rs/R0
  uart_send_string("Ratio Rs/R0: ");
  uart_send_float(datosGas.rs_ro_ratio, 2);
  uart_send_string("\r\n");
  
  // Gas Detectado
  uart_send_string("Gas Detectado: ");
  uart_send_string(datosGas.nombre_gas.c_str());
  
  // PPM
  uart_send_string(" | PPM: ");
  uart_send_float(datosGas.ppm, 1);
  uart_send_string("\r\n");
  
  // Calibración
  uart_send_string("Calibracion: K=");
  uart_send_float(K, 6);
  uart_send_string(", R0=");
  uart_send_float(R0, 2);
  uart_send_string("\r\n");
  
  // Footer
  uart_send_string("============================================\r\n\r\n");
}

// ----------------------------------------------------------
// ------------------- Funciones de Utilidad ----------------
// ----------------------------------------------------------

float convertirAVoltios(int lecturaADC) {
  return K * lecturaADC;
}

float calcularR0DesdeVoltaje(float voltaje) {
  float Rs_aire = (VCC - voltaje) * RL / voltaje;
  return Rs_aire / 9.8; // Valor típico para MQ-2 en aire limpio
}

float calcularRs(float voltaje) {
  if (voltaje <= 0) return RL * 1000; // Evitar división por cero
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

// Funciones auxiliares para UART
void uart_send_float(float value, int decimals) {
  char buffer[20];
  // Usamos dtostrf para convertir float a string
  dtostrf(value, 0, decimals, buffer);
  uart_write_bytes(UART_NUM, (const char*)buffer, strlen(buffer));
}

void uart_send_int(int value) {
  char buffer[20];
  // Usamos itoa como sugiere la NOTA2
  itoa(value, buffer, 10);
  uart_write_bytes(UART_NUM, (const char*)buffer, strlen(buffer));
}

void uart_send_string(const char* str) {
  uart_write_bytes(UART_NUM, str, strlen(str));
}