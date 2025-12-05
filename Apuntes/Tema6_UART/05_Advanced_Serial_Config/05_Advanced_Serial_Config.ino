
// 5. Configuración Avanzada del Puerto Serie (para M5Stack)
#include <M5Stack.h>

void setup() {
  // Primero, inicializa el M5Stack.
  M5.begin();

  // --- Configuración Avanzada de Serial.begin() ---
  // Para esta demostración, usaremos una configuración no estándar:
  // 8 bits de datos, paridad PAR (Even), 1 bit de parada (SERIAL_8E1).
  Serial.begin(9600, SERIAL_8E1);
  delay(100);

  // Muestra la configuración en la pantalla del M5Stack
  M5.Lcd.fillScreen(BLACK);
  M5.Lcd.setCursor(0, 0);
  M5.Lcd.setTextSize(2);
  M5.Lcd.println("Ejemplo 5: Config. Avanzada");
  M5.Lcd.setTextSize(1);
  M5.Lcd.println("Serial configurado a 9600 baudios, 8E1.");
  M5.Lcd.println("Ajusta tu Monitor Serie a esta configuracion para ver el texto correctamente.");

  Serial.println("Puerto Serie configurado con 8 bits de datos, paridad PAR, 1 bit de parada (8E1).");
  Serial.println("Si puedes leer esto, tu Monitor Serie esta configurado correctamente.");
}

void loop() {
  M5.update();
  
  if (Serial.available() > 0) {
    char receivedChar = Serial.read();
    
    // Muestra en Monitor Serie
    Serial.print("Recibido: ");
    Serial.println(receivedChar);

    // Muestra en M5Stack
    M5.Lcd.setCursor(0, 50);
    M5.Lcd.print("Recibido: ");
    M5.Lcd.print(receivedChar);
  }
}
