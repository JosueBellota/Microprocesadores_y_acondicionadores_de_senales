
// 4. Envío de Datos con Formato (para M5Stack)
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
  M5.Lcd.println("Ejemplo 4: Envio Formateado");
  M5.Lcd.setTextSize(1);
  
  Serial.println("Demostracion de envio de datos con formato.");
  Serial.println("Se enviara el numero 78 en diferentes bases cada 3 segundos.");
  Serial.println("---");
}

void loop() {
  // Actualiza el estado del M5
  M5.update();
  
  int numero = 78;

  // --- Envío en Diferentes Bases Numéricas (Monitor Serie) ---
  Serial.print("Decimal: "); Serial.println(numero);
  Serial.print("Binario: "); Serial.println(numero, BIN);
  Serial.print("Hexadecimal: "); Serial.println(numero, HEX);
  Serial.print("Octal: "); Serial.println(numero, OCT);
  Serial.println("---");

  // --- Envío en Diferentes Bases Numéricas (Pantalla M5Stack) ---
  M5.Lcd.setCursor(0, 20);
  M5.Lcd.printf("Numero: %d\n\n", numero);
  M5.Lcd.printf("BIN: %s\n", String(numero, BIN).c_str());
  M5.Lcd.printf("HEX: %s\n", String(numero, HEX).c_str());
  M5.Lcd.printf("OCT: %s\n", String(numero, OCT).c_str());

  delay(3000);
}
