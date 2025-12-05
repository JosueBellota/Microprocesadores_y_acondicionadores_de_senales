/* Ejer. 3 I2C */

#include <M5Stack.h>
//#include "Wire.h"
#define BMP180_I2CADDR 0x77
#define BMP180_CONTROL 0xF4 
#define BMP180_DATA 0xF6
#define BMP180_IDREG 0xD0

// Declaración de funciones
uint8_t read_1byte(int device_addr, int read_addr);
void write_1byte(int device_addr, int write_addr, int data);

void setup() {
  // put your setup code here, to run once:
  M5.begin();
  Wire.begin();
  Serial.begin(115200);
  M5.Lcd.setTextSize(2);
  M5.Lcd.printf("Ejerc. 3 I2C\r\n");
  M5.Lcd.printf("\r\n");
  M5.Lcd.printf("BMP180:\r\n");
  Serial.println("  ");
  Serial.println("------------------------------------");
  Serial.println("Ejer. 3 I2C");
  Serial.println("------------------------------------");
}


void loop() {
  // put your main code here, to run repeatedly:
  M5.BtnA.read();
  //M5.update();
  uint8_t dato = 0x2A;
  if(M5.BtnA.wasPressed()) {
    write_1byte(BMP180_I2CADDR,BMP180_CONTROL,dato);
    //dato = read_1byte(BMP180_I2CADDR,BMP180_CONTROL);
    Serial.print("VALOR ESCRITO BMP180_CONTROLREG = 0x");
    Serial.println(dato,HEX);
    // Si se lee una vez pasa el tiempo de conversión el bit de 
    // start se pone a cero (es un flag)
    //delay(100); 
    dato = read_1byte(BMP180_I2CADDR,BMP180_CONTROL);
    Serial.print("VALOR LEIDO BMP180_CONTROLREG = 0x");
    Serial.println(dato,HEX);
  }
  if (Serial.available()>0){
    char ch = Serial.read();
	if(ch=='A') {
    write_1byte(BMP180_I2CADDR,BMP180_CONTROL,dato);
    //dato = read_1byte(BMP180_I2CADDR,BMP180_CONTROL);
    Serial.print("VALOR ESCRITO BMP180_CONTROLREG = 0x");
    Serial.println(dato,HEX);
    // Si se lee una vez pasa el tiempo de conversión el bit de 
    // start se pone a cero (es un flag)
    //delay(100); 
    dato = read_1byte(BMP180_I2CADDR,BMP180_CONTROL);
    Serial.print("VALOR LEIDO BMP180_CONTROLREG = 0x");
    Serial.println(dato,HEX);		
	}
  }
   delay(1000); 
}

void write_1byte(int device_addr, int write_addr, int data) {
  // Escribir la dirección del registro que se quiere escribir 
  // y después escribir su valor  
  
  
  
  
  
 }

uint8_t read_1byte(int device_addr, int read_addr){
  uint8_t ret;
  // 1ª FASE: Escribir la dirección del registro que se quiere leer


  // 2ª FASE: Leer el registro
  
  return ret;
}
