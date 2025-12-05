
// 3. Uso de Múltiples Temporizadores (para M5Stack)
// El ESP32 tiene 4 temporizadores de hardware. Este ejemplo muestra cómo usar dos de ellos
// simultáneamente para realizar tareas a diferentes intervalos.

#include <M5Stack.h>

// --- Variables para el Timer 0 ---
hw_timer_t *timer0 = NULL;
volatile bool flag0 = false;

// --- Variables para el Timer 1 ---
hw_timer_t *timer1 = NULL;
volatile bool flag1 = false;


// --- ISRs ---
void IRAM_ATTR onTimer0() {
  flag0 = true;
}

void IRAM_ATTR onTimer1() {
  flag1 = true;
}


void setup() {
  M5.begin();
  Serial.begin(115200);

  M5.Lcd.fillScreen(BLACK);
  M5.Lcd.setTextSize(2);
  M5.Lcd.setCursor(0, 0);
  M5.Lcd.println("Ejemplo 3: Multiples Timers");
  
  // --- Configuración del Timer 0 (1 segundo) ---
  timer0 = timerBegin(0, 80, true); // Timer 0, 1 tick = 1 µs
  timerAttachInterrupt(timer0, &onTimer0, true);
  timerAlarmWrite(timer0, 1000000, true); // 1 segundo
  timerAlarmEnable(timer0);
  Serial.println("Timer 0 configurado para 1 segundo.");

  // --- Configuración del Timer 1 (2.5 segundos) ---
  timer1 = timerBegin(1, 80, true); // Timer 1, 1 tick = 1 µs
  timerAttachInterrupt(timer1, &onTimer1, true);
  timerAlarmWrite(timer1, 2500000, true); // 2.5 segundos
  timerAlarmEnable(timer1);
  Serial.println("Timer 1 configurado para 2.5 segundos.");
}


void loop() {
  M5.update();

  // --- Manejo de la Interrupción del Timer 0 ---
  if (flag0) {
    flag0 = false;
    
    // Cada segundo, imprime un punto en la parte superior de la pantalla
    static int dotCount = 0;
    M5.Lcd.setTextSize(2);
    M5.Lcd.setCursor(10 + (dotCount * 15), 50);
    M5.Lcd.print(".");
    dotCount++;
    if (dotCount > 20) {
      dotCount = 0;
      M5.Lcd.fillRect(0, 50, 320, 20, BLACK);
    }
    Serial.println("Timer 0 Tick!");
  }

  // --- Manejo de la Interrupción del Timer 1 ---
  if (flag1) {
    flag1 = false;

    // Cada 2.5 segundos, cambia el color de un rectángulo en la parte inferior
    static int colorIndex = 0;
    uint16_t colors[] = {RED, GREEN, BLUE, YELLOW, MAGENTA};
    M5.Lcd.fillRect(80, 120, 160, 80, colors[colorIndex]);
    colorIndex = (colorIndex + 1) % 5; // Cicla entre los 5 colores
    Serial.println("Timer 1 Tock!");
  }
}
