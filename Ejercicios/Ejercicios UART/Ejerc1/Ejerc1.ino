/* CLASE UART Ejem_1*/

#include <M5Stack.h>

#define TX1_pin  21
#define RX1_pin  -1


//HardwareSerial Serial1(1);
//HardwareSerial Serial2(2);

void setup() {
  /*
  First Parameter (1): Enables or disables the internal display. Setting it to 1 means the display is enabled.
  Second Parameter (0): Configures the serial output. 0 typically means that serial communication is disabled.
  Third Parameter (0): Configures the SD card functionality. 0 means the SD card is not initialized.
  Fourth Parameter (1): Enables or disables the I2C interface. Setting it to 1 enables I2C communication.
  */
  M5.begin(1, 0, 0, 1);
  M5.Lcd.setTextSize(3);
  M5.Lcd.printf("\n\n\n\n");
  M5.Lcd.printf("   Test UARTs\r\n");

  // Inincializacion UARTS
  Serial.begin(115200); //UART0
  Serial1.begin(115200, SERIAL_8N1, RX1_pin, TX1_pin); //UART1


  Serial.println("  ");
  Serial.println("------------------------------------");
  Serial.println("Prueba de test UARTs");
  Serial.println("------------------------------------");

}

void loop() {

  int A = 135;
  if (Serial.available() > 0) {
    char ch = Serial.read(); 
    if (ch == '1') {
      Serial.print("A"); 
      Serial1.print("A"); 
    }
    if (ch == '2') {
      Serial.println("Hola");
      Serial1.print("Hola");
    }
    if (ch == '3') {
      Serial.print(A);
      Serial1.print(A);
    }
    if (ch == '4') {
      Serial.print(A, 0); 
      Serial1.print(A, 0);
    }
    if (ch == '5') {
      Serial.print(A, BIN); 
      Serial1.print(A, BIN);
    }
  }
}
