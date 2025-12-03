
// 6. Uso de Múltiples Puertos Serie (para M5Stack)
#include <M5Stack.h>

// El ESP32 tiene 3 UARTs. UART0 es 'Serial'. Usaremos UART2 para 'MySerial'.
// (UART1 a menudo se usa para la memoria PSRAM, por lo que UART2 es una opción más segura).
HardwareSerial MySerial(2);

void setup() {
  // Inicializa el M5Stack.
  M5.begin();

  // Inicia el puerto serie principal (conectado al monitor USB).
  Serial.begin(115200);
  delay(100);

  // --- Iniciar el Segundo Puerto Serie (UART2) ---
  // Para este ejemplo, usaremos los pines GPIO 16 (RX) y 17 (TX).
  // Estos son pines comunes y están disponibles en el M5Stack.
  int rxPin = 16;
  int txPin = 17;
  MySerial.begin(9600, SERIAL_8N1, rxPin, txPin);

  // Muestra información en la pantalla del M5Stack.
  M5.Lcd.fillScreen(BLACK);
  M5.Lcd.setCursor(0, 0);
  M5.Lcd.setTextSize(2);
  M5.Lcd.println("Ejemplo 6: Multiples Puertos");
  M5.Lcd.setTextSize(1);
  M5.Lcd.printf("UART0 (Monitor) @ 115200\n");
  M5.Lcd.printf("UART2 @ 9600 (RX:%d, TX:%d)\n", rxPin, txPin);
  M5.Lcd.println("\nPasarela Serie Activa.");

  Serial.println("Monitor Serie (UART0) inicializado.");
  Serial.println("Puerto Serie 2 (UART2) inicializado.");
  Serial.println("Actuando como pasarela (gateway)...");
}

void loop() {
  M5.update();

  // --- Pasarela Serie (Serial Gateway) ---
  // De Monitor USB (Serial) a UART2 (MySerial)
  if (Serial.available()) {
    char c = Serial.read();
    MySerial.write(c);
    M5.Lcd.setCursor(0, 60);
    M5.Lcd.print("-> UART2: ");
    M5.Lcd.print(c);
  }

  // De UART2 (MySerial) a Monitor USB (Serial)
  if (MySerial.available()) {
    char c = MySerial.read();
    Serial.write(c);
    M5.Lcd.setCursor(0, 80);
    M5.Lcd.print("<- UART0: ");
    M5.Lcd.print(c);
  }
}
