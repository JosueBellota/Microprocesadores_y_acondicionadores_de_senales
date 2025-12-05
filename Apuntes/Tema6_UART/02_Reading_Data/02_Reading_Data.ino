
// 2. Lectura de Datos del Puerto Serie (para M5Stack)
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
  M5.Lcd.println("Ejemplo 2: Leer Datos");
  M5.Lcd.setTextSize(1);
  M5.Lcd.println("Envia caracteres desde el Monitor Serie.");

  Serial.println("Listo para recibir datos. Envia un caracter a la vez.");
}

void loop() {
  // Actualiza el estado del M5
  M5.update();

  // --- Comprobar si hay Datos Disponibles ---
  if (Serial.available() > 0) {
    
    // --- Leer un Byte a la Vez ---
    char incomingByte = Serial.read();

    // --- Mostrar los Datos Recibidos en el Monitor Serie---
    Serial.print("Caracter recibido: ");
    Serial.println(incomingByte);
    Serial.print("Codigo ASCII: ");
    Serial.println(incomingByte, DEC);
    Serial.println("---");

    // --- Mostrar los Datos Recibidos en la pantalla del M5Stack ---
    M5.Lcd.fillScreen(BLACK);
    M5.Lcd.setCursor(0, 0);
    M5.Lcd.setTextSize(2);
    M5.Lcd.print("Recibido: ");
    M5.Lcd.println(incomingByte);
    M5.Lcd.setTextSize(1);
    M5.Lcd.print("ASCII: ");
    M5.Lcd.println(incomingByte, DEC);
  }
}
