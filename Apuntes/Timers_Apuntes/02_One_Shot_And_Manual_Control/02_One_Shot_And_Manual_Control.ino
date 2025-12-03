
// 2. Temporizador de Disparo Único y Control Manual (para M5Stack)
// Este ejemplo demuestra cómo usar un temporizador que se dispara una sola vez
// y cómo controlarlo manualmente con los botones del M5Stack.

#include <M5Stack.h>

hw_timer_t *timer = NULL;
volatile bool alarmFlag = false;

void IRAM_ATTR onTimer() {
  // La interrupción se ha disparado
  alarmFlag = true;
}

void setup() {
  M5.begin();
  Serial.begin(115200);

  M5.Lcd.fillScreen(BLACK);
  M5.Lcd.setCursor(0, 0);
  M5.Lcd.setTextSize(2);
  M5.Lcd.println("Ejemplo 2: Disparo Unico");
  M5.Lcd.setTextSize(1);
  M5.Lcd.println("Boton A: Iniciar Timer (3 seg)");
  M5.Lcd.println("Boton B: Detener Timer");

  // --- Configuración del Temporizador ---
  // Timer 0, prescaler de 80 (1 tick = 1 µs)
  timer = timerBegin(0, 80, true);
  
  // Asocia la ISR
  timerAttachInterrupt(timer, &onTimer, true);
  
  // Establece la alarma para 3 segundos (3,000,000 µs)
  // El último parámetro 'false' significa que NO se reiniciará automáticamente.
  // Es un temporizador de "disparo único" (one-shot).
  timerAlarmWrite(timer, 3000000, false);
  
  // NO habilitamos la alarma aquí. Lo haremos manualmente.
  // timerAlarmEnable(timer);

  Serial.println("Temporizador configurado para un disparo unico en 3 segundos.");
  Serial.println("Presiona el Boton A para iniciarlo.");
}

void loop() {
  M5.update(); // Lee el estado de los botones

  // --- Control Manual del Temporizador ---

  // Botón A: Inicia el temporizador
  if (M5.BtnA.wasPressed()) {
    // timerStart() comienza la cuenta. Si el temporizador ya estaba corriendo,
    // se reinicia desde el principio.
    timerStart(timer);
    Serial.println("Boton A: Temporizador iniciado!");
    M5.Lcd.fillRect(0, 60, 320, 180, BLACK);
    M5.Lcd.setCursor(10, 100);
    M5.Lcd.setTextColor(YELLOW);
    M5.Lcd.setTextSize(2);
    M5.Lcd.println("Contando 3 segundos...");
    M5.Lcd.setTextColor(WHITE);
  }

  // Botón B: Detiene el temporizador
  if (M5.BtnB.wasPressed()) {
    // timerStop() detiene la cuenta.
    timerStop(timer);
    Serial.println("Boton B: Temporizador detenido!");
    M5.Lcd.fillRect(0, 60, 320, 180, BLACK);
    M5.Lcd.setCursor(10, 100);
    M5.Lcd.setTextColor(RED);
    M5.Lcd.setTextSize(2);
    M5.Lcd.println("TEMPORIZADOR DETENIDO");
    M5.Lcd.setTextColor(WHITE);
  }

  // --- Manejo de la Alarma ---
  if (alarmFlag) {
    alarmFlag = false; // Resetea el flag
    
    Serial.println("ALARMA! El temporizador ha terminado.");
    M5.Lcd.fillRect(0, 60, 320, 180, BLACK);
    M5.Lcd.setCursor(10, 100);
    M5.Lcd.setTextColor(GREEN);
    M5.Lcd.setTextSize(3);
    M5.Lcd.println("¡ALARMA!");
    M5.Lcd.setTextColor(WHITE);
    // Como auto-reload es 'false', el temporizador no volverá a dispararse
    // hasta que se llame a timerStart() de nuevo.
  }
}
