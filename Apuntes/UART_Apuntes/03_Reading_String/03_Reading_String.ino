// 3. Lectura de Cadenas de Texto (para M5Stack)
#include <M5Stack.h>

void setup() {
  // Primero, inicializa el M5Stack.
  M5.begin();

  // Inicia la comunicación serie a 115200 baudios.
  Serial.begin(115200);
  delay(100);

  // Muestra un mensaje en la pantalla del M5Stack
  M5.Lcd.fillScreen(BLACK);
  M5.Lcd.setCursor(0, 0);
  M5.Lcd.setTextSize(2);
  M5.Lcd.println("Ejemplo 3: Leer Cadenas");
  M5.Lcd.setTextSize(1);
  M5.Lcd.println("Escribe un mensaje y presiona Enter en el Monitor Serie.");

  Serial.println("Listo para recibir cadenas de texto.");
}

void loop() {
  // Actualiza el estado del M5
  M5.update();

  if (Serial.available() > 0) {
    String incomingString = Serial.readStringUntil('\n');
    incomingString.trim();

    if (incomingString.length() > 0) {
      // Muestra en el Monitor Serie
      Serial.print("Cadena recibida: ");
      Serial.println(incomingString);

      // Muestra en la pantalla del M5Stack
      M5.Lcd.fillScreen(BLACK);
      M5.Lcd.setCursor(0, 0);
      M5.Lcd.setTextSize(2);
      M5.Lcd.print("Recibido:");
      M5.Lcd.setCursor(0, 20);
      M5.Lcd.print(incomingString);

      if (incomingString == "ON") {
        Serial.println("Comando reconocido: ENCENDER");
        M5.Lcd.setCursor(0, 40);
        M5.Lcd.setTextColor(GREEN);
        M5.Lcd.print("LED ENCENDIDO");
        M5.Lcd.setTextColor(WHITE);
      } else if (incomingString == "OFF") {
        Serial.println("Comando reconocido: APAGAR");
        M5.Lcd.setCursor(0, 40);
        M5.Lcd.setTextColor(RED);
        M5.Lcd.print("LED APAGADO");
        M5.Lcd.setTextColor(WHITE);
      }
    }
  }
}
