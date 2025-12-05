/* Ejer. 4 I2C */

#include <M5Stack.h>
//#include "Wire.h"
#define BMP180_I2CADDR 0x77
#define BMP180_CONTROL 0xF4 
#define BMP180_DATA 0xF6
#define BMP180_IDREG 0xD0

// Declaración de función
uint16_t read_2bytes(int device_addr, int read_addr);

void setup() {
  // put your setup code here, to run once:
  M5.begin();
  Wire.begin();
  Serial.begin(115200);
  M5.Lcd.setTextSize(2);
  M5.Lcd.printf("Ejerc. 4 I2C:\r\n");
  M5.Lcd.printf("\r\n");
  M5.Lcd.printf("BMP180:\r\n");
  Serial.println("  ");
  Serial.println("------------------------------------");
  Serial.println("Ejerc. 4 I2C");
  Serial.println("------------------------------------");
}


void loop() {
  // put your main code here, to run repeatedly:
  M5.BtnA.read();
  uint16_t data;
  if(M5.BtnA.wasPressed()) {
    data = read_2bytes(BMP180_I2CADDR,BMP180_DATA);
    Serial.print("BMP180_TEMPDATA = 0x");
    Serial.println(data,HEX);
  }
  if (Serial.available()>0){
    char ch = Serial.read();
	if(ch=='A') {
		data = read_2bytes(BMP180_I2CADDR,BMP180_DATA);
		Serial.print("BMP180_TEMPDATA_HEX = 0x");
		Serial.println(data,HEX);		
    Serial.print("BMP180_TEMPDATA_DEC = ");
    Serial.println(data,DEC);
	}
  }
   
}


uint16_t read_2bytes(int device_addr, int read_addr) {
  uint16_t ret;
  // 1ª FASE: Escribir la dirección del registro que se quiere leer
  Wire.beginTransmission(device_addr);  
  Wire.write(read_addr); 
  Wire.endTransmission();
  // 2ª FASE: Leer el registro
  Wire.requestFrom(device_addr,2);
  ret = Wire.read();
  ret <<= 8;
  ret |= Wire.read(); 
  return ret;
}
