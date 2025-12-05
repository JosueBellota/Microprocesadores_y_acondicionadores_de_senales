/* Ejer. 1 I2C */

#include <M5Stack.h>
//#include "Wire.h"
#define BMP180_I2CADDR 0x77
#define BMP180_CONTROL 0xF4 
#define BMP180_DATA 0xF6
#define BMP180_IDREG 0xD0
#define SDA 21
#define SCL 22

// Declaración de la función
uint8_t read_1byte(int device_addr, int read_addr);

void setup() {
  M5.begin();
  Serial.begin(115200);
  M5.Lcd.setTextSize(3);
  M5.Lcd.printf("Ejer 1 I2C \r\n");
  M5.Lcd.printf("BMP180:\r\n");
  Serial.println("  ");
  Serial.println("------------------------------------");
  Serial.println("Ejer. 1: leer registro de  BMP180");
  Serial.println("------------------------------------");
  
  // Configura pines I2C como master
  
  

}

void loop() {
  // put your main code here, to run repeatedly:
  M5.BtnA.read();
  //M5.update();
  uint8_t data;
  if (M5.BtnA.wasPressed()) {
    data = read_1byte(BMP180_I2CADDR, BMP180_IDREG);
    Serial.print("BMP180_ID = 0x");
    Serial.println(data, HEX);
  }
  if (Serial.available() > 0) {
    char ch = Serial.read();
    if (ch == 'A') {
      data = read_1byte(BMP180_I2CADDR, BMP180_IDREG);
      Serial.print("BMP180_ID = 0x");
      Serial.println(data, HEX);
    }
  }
}


uint8_t read_1byte(int device_addr, int read_addr) {
  uint8_t ret;
  // 1ª FASE: Escribir la dirección del registro que se quiere leer



  // 2ª FASE: Leer el registro


  
  return ret;
}
