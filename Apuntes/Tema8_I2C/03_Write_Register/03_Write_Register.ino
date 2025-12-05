#include <M5Stack.h>
#include <Wire.h>

/*
  Conexión del sensor BMP180 con el M5Stack:
  - M5Stack 3.3V -> BMP180 VIN
  - M5Stack GND  -> BMP180 GND
  - M5Stack G21 (SDA) -> BMP180 SDA
  - M5Stack G22 (SCL) -> BMP180 SCL
*/

// Dirección del BMP180
#define BMP180_ADDR 0x77
// Dirección del registro de control del BMP180
#define BMP180_CTRL_REG 0xF4
// Valor para iniciar la conversión de temperatura
#define BMP180_START_TEMP_CONV 0x2E

// Función para escribir 1 byte en un registro de un dispositivo I2C
void write_1byte(uint8_t device_addr, uint8_t write_addr, uint8_t data) {
  Wire.beginTransmission(device_addr);
  Wire.write(write_addr);
  Wire.write(data);
  Wire.endTransmission();
}

// Función para leer 1 byte de un registro de un dispositivo I2C
uint8_t read_1byte(uint8_t device_addr, uint8_t read_addr) {
  uint8_t read_data;
  Wire.beginTransmission(device_addr);
  Wire.write(read_addr);
  Wire.endTransmission(false);
  Wire.requestFrom(device_addr, (uint8_t)1);
  if (Wire.available()) {
    read_data = Wire.read();
  } else {
    read_data = 0; // Error
  }
  return read_data;
}

void setup() {
  M5.begin();
  Wire.begin();
  Serial.begin(115200);

}

void loop() {
  M5.update();

  if (M5.BtnA.wasPressed()) {
  

    // Escribir en el registro de control para iniciar la conversión de temperatura
    write_1byte(BMP180_ADDR, BMP180_CTRL_REG, BMP180_START_TEMP_CONV);

    Serial.println("Comando de conversion de temperatura enviado.");
    delay(10); // Pequeña espera

    // Leer el registro de control para verificar la escritura
    uint8_t ctrl_reg_value = read_1byte(BMP180_ADDR, BMP180_CTRL_REG);

    Serial.println(ctrl_reg_value, HEX);

    if (ctrl_reg_value == BMP180_START_TEMP_CONV) {
      Serial.println("Verificacion correcta");
    } else {
      Serial.println("Verificacion fallida:");
    }
  }
}
