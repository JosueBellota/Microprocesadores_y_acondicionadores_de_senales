

// 4. Cambio Dinámico del Valor de Alarma (para M5Stack)
// Este ejemplo demuestra cómo cambiar la frecuencia de una interrupción de temporizador
// en tiempo de ejecución usando los botones A y B del M5Stack.

#include <M5Stack.h>

hw_timer_t *timer = NULL;
volatile bool flag = false;

// El valor de la alarma se declara globalmente para poder modificarlo
uint64_t alarmValue = 1000000; // Valor inicial: 1 segundo

void IRAM_ATTR onTimer() {
  flag = true;
}

void setup() {
  M5.begin();
  Serial.begin(115200);

  M5.Lcd.fillScreen(BLACK);
  M5.Lcd.setTextSize(2);
  M5.Lcd.setCursor(0, 0);
  M5.Lcd.println("Ejemplo 4: Alarma Dinamica");
  M5.Lcd.setTextSize(1);
  M5.Lcd.println("Boton A: Mas rapido");
  M5.Lcd.println("Boton B: Mas lento");
  
  // --- Configuración del Temporizador ---
  timer = timerBegin(0, 80, true); // Timer 0, 1 tick = 1 µs
  timerAttachInterrupt(timer, &onTimer, true);
  
  // Escribimos el valor inicial de la alarma
  timerAlarmWrite(timer, alarmValue, true); // Auto-reload habilitado
  timerAlarmEnable(timer);

  Serial.printf("Timer configurado con alarma inicial de %llu us\n", alarmValue);
  M5.Lcd.printf("Intervalo inicial: %d ms\n", alarmValue / 1000);
}

void loop() {
  M5.update();

  // --- Cambiar el valor de la alarma dinámicamente ---
  
  // Botón A: Reduce el intervalo (más rápido)
  if (M5.BtnA.wasPressed()) {
    if (alarmValue > 100000) { // Límite mínimo de 100ms
      alarmValue -= 100000; // Resta 100ms
      timerAlarmWrite(timer, alarmValue, true); // Actualiza el temporizador con el nuevo valor
      Serial.printf("Nuevo intervalo: %llu us\n", alarmValue);
      M5.Lcd.fillRect(0, 60, 320, 20, BLACK);
      M5.Lcd.setCursor(0, 60);
      M5.Lcd.printf("Intervalo: %d ms", alarmValue / 1000);
    }
  }

  // Botón B: Aumenta el intervalo (más lento)
  if (M5.BtnB.wasPressed()) {
    if (alarmValue < 5000000) { // Límite máximo de 5s
      alarmValue += 100000; // Suma 100ms
      timerAlarmWrite(timer, alarmValue, true); // Actualiza el temporizador
      Serial.printf("Nuevo intervalo: %llu us\n", alarmValue);
      M5.Lcd.fillRect(0, 60, 320, 20, BLACK);
      M5.Lcd.setCursor(0, 60);
      M5.Lcd.printf("Intervalo: %d ms", alarmValue / 1000);
    }
  }

  // --- Manejo de la Interrupción ---
  if (flag) {
    flag = false;

    // Haz parpadear un círculo en el centro de la pantalla
    static bool circleState = false;
    circleState = !circleState;
    if (circleState) {
      M5.Lcd.fillCircle(160, 150, 50, ORANGE);
    } else {
      M5.Lcd.fillCircle(160, 150, 50, BLACK);
    }
  }
}
