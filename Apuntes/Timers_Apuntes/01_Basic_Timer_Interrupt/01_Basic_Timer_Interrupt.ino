
// 1. Interrupción Básica de Temporizador (para M5Stack)
// Este ejemplo muestra cómo configurar un temporizador de hardware del ESP32
// para generar una interrupción periódica.

#include <M5Stack.h>

// --- Variables del Temporizador ---
hw_timer_t *timer = NULL; // Puntero para el objeto del temporizador
volatile bool interruptFlag = false; // Flag para ser activado por la interrupción

// --- Interrupt Service Routine (ISR) ---
// Esta es la función que se llamará cuando el temporizador alcance su valor de alarma.
// ¡Debe ser lo más rápida posible! Por eso usamos un 'flag'.
// El atributo IRAM_ATTR asegura que la función se cargue en la RAM para una ejecución más rápida.
void IRAM_ATTR onTimer() {
  interruptFlag = true;
}

void setup() {
  M5.begin();
  Serial.begin(115200);

  M5.Lcd.fillScreen(BLACK);
  M5.Lcd.setCursor(0, 0);
  M5.Lcd.setTextSize(2);
  M5.Lcd.println("Ejemplo 1: Timer Basico");

  // --- Configuración del Temporizador ---
  // El ESP32 tiene 4 temporizadores de hardware (0 a 3). Usaremos el temporizador 0.
  // El prescaler divide el reloj base de 80MHz. 
  // Un prescaler de 80 da un 'tick' cada 1 microsegundo (80MHz / 80 = 1MHz).
  uint16_t prescaler = 80;
  timer = timerBegin(0, prescaler, true); // Timer 0, prescaler de 80, contar hacia arriba (true)

  // Asocia la función 'onTimer' a la interrupción del temporizador.
  timerAttachInterrupt(timer, &onTimer, true); // true para interrupción de flanco (edge)

  // Establece el valor de alarma en 'ticks'.
  // 1,000,000 de ticks = 1 segundo (con prescaler de 80).
  // El último parámetro 'true' significa que el temporizador se reiniciará automáticamente (auto-reload).
  uint64_t alarmValue = 1000000;
  timerAlarmWrite(timer, alarmValue, true);

  // Habilita la alarma del temporizador.
  timerAlarmEnable(timer);

  Serial.println("Temporizador configurado para una interrupcion cada 1 segundo.");
  M5.Lcd.setTextSize(1);
  M5.Lcd.println("Interrupcion cada 1 seg.");
}

void loop() {
  M5.update();

  // Comprueba si la interrupción ha ocurrido.
  if (interruptFlag) {
    // Resetea el flag para la próxima interrupción.
    interruptFlag = false;

    // --- Haz algo aquí ---
    // Este es un buen lugar para ejecutar código que no es crítico en tiempo.
    static bool ledState = false;
    ledState = !ledState;
    
    // En lugar de un LED, vamos a cambiar el color del fondo de la pantalla.
    if (ledState) {
      M5.Lcd.fillRect(0, 50, 320, 190, BLUE);
      Serial.println("Tick! Pantalla Azul");
    } else {
      M5.Lcd.fillRect(0, 50, 320, 190, RED);
      Serial.println("Tock! Pantalla Roja");
    }
  }
}
