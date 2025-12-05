/*
  Conexión del sensor BMP180 con el M5Stack:
  - M5Stack 3.3V -> BMP180 VIN
  - M5Stack GND  -> BMP180 GND
  - M5Stack G21 (SDA) -> BMP180 SDA
  - M5Stack G22 (SCL) -> BMP180 SCL
*/
#include <M5Stack.h>
#include <Wire.h>

// Dirección del BMP180
#define BMP180_ADDR 0x77
// Dirección del registro de ID del BMP180
#define BMP180_ID_REG 0xD0

// Función para leer 1 byte de un registro de un dispositivo I2C
uint8_t read_1byte(uint8_t device_addr, uint8_t read_addr) {
  uint8_t read_data;
  
  // Inicia la transmisión a la dirección del dispositivo
  Wire.beginTransmission(device_addr);
  // Escribe la dirección del registro que se quiere leer
  Wire.write(read_addr);
  // Termina la transmisión, pero no envía una señal de stop (el 'false' es importante)
  Wire.endTransmission(false);
  
  // Solicita 1 byte desde el dispositivo
  Wire.requestFrom(device_addr, (uint8_t)1);
  
  // Lee el byte si está disponible
  if (Wire.available()) {
    read_data = Wire.read();
  } else {
    read_data = 0; // O algún valor de error
  }
  
  return read_data;
}

void setup() {
  // Inicializa el M5Stack
  M5.begin();
  
  // Inicializa la comunicación I2C
  Wire.begin();
  
  // Inicializa la comunicación serie
  Serial.begin(115200);
  
  Serial.println("M5Stack initializing...OK");
  Serial.println("Prueba de control del sensor BMP180");
  Serial.println("Pulsa el boton A para leer el ID del BMP180");
}

void loop() {
  // Actualiza el estado de los botones
  M5.update();
  
  // Si se pulsa el botón A
  if (M5.BtnA.wasPressed()) {
    // Lee el ID del BMP180
    uint8_t bmp_id = read_1byte(BMP180_ADDR, BMP180_ID_REG);
    
    // Muestra el ID por el monitor serie
    Serial.print("BMP180 ID = 0x");
    Serial.println(bmp_id, HEX);
  }
}