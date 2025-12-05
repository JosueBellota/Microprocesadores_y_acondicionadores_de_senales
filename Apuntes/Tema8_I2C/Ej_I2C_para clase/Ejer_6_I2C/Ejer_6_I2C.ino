/* Ejer. 6 I2C */

#include <M5Stack.h>
#include <driver\i2c.h>

#define BMP180_I2CADDR 0x77
#define BMP180_CONTROL 0xF4 
#define BMP180_DATA 0xF6
#define BMP180_IDREG 0xD0
#define SDA 21
#define SCL 22
uint8_t read_1byte(uint8_t device_addr, uint8_t read_addr);

void setup() {
  // put your setup code here, to run once:
  M5.begin();
  //Wire.begin(SDA,SCL);
  Serial.begin(115200);
  M5.Lcd.setTextSize(2);
  M5.Lcd.printf("Ejerc. 6 I2C\r\n");
  M5.Lcd.printf("\r\n");
  M5.Lcd.printf("BMP180:\r\n");
  Serial.println("  ");
  Serial.println("------------------------------------");
  Serial.println("Ejerc. 6 I2C");
  Serial.println("------------------------------------");

  // Parámetros de onfiguración del driver I2C 0 
  // en master mode, velocidad normal 
  i2c_config_t conf;
  conf.mode = I2C_MODE_MASTER;
  conf.sda_io_num = gpio_num_t(SDA);
  conf.scl_io_num = gpio_num_t(SCL);
  conf.sda_pullup_en = GPIO_PULLUP_ENABLE;
  conf.scl_pullup_en = GPIO_PULLUP_ENABLE;
  conf.master.clk_speed = 100000;
  
  // Configurar el driver
  i2c_param_config(I2C_NUM_0, &conf);

  // Instalar el driver
  i2c_driver_install(I2C_NUM_0, I2C_MODE_MASTER, 0, 0, 0);
}


void loop() {
  M5.BtnA.read();
  
  uint8_t data;
  if(M5.BtnA.wasPressed()) {
    data = read_1byte(BMP180_I2CADDR,BMP180_IDREG);
    Serial.print("BMP180_ID = 0x");
    Serial.println(data,HEX);
  }
  if (Serial.available()>0){
    char ch = Serial.read();
	if(ch=='A') {
		data = read_1byte(BMP180_I2CADDR,BMP180_IDREG);
		Serial.print("BMP180_ID = 0x");
		Serial.println(data,HEX);	
	}
  }
   
}


uint8_t read_1byte(uint8_t device_addr, uint8_t read_addr) {
  uint8_t ret;
  
  // 1ª FASE: Escribir la dirección del registro que se quiere leer
  i2c_cmd_handle_t cmd = i2c_cmd_link_create();
  i2c_master_start(cmd);
  i2c_master_write_byte(cmd, (device_addr << 1) | I2C_MASTER_WRITE, true);
  i2c_master_write_byte(cmd, read_addr, true);
  i2c_master_stop(cmd);
  i2c_master_cmd_begin(I2C_NUM_0, cmd, 1000 / portTICK_RATE_MS);
  i2c_cmd_link_delete(cmd);
//  Wire.beginTransmission(device_addr);  
//  Wire.write(read_addr); 
//  Wire.endTransmission();

  // 2ª FASE: Leer el registro
  cmd = i2c_cmd_link_create();
  i2c_master_start(cmd);
  i2c_master_write_byte(cmd, (device_addr << 1) | I2C_MASTER_READ, true);
  i2c_master_read_byte(cmd, &ret, (i2c_ack_type_t) true);
  i2c_master_stop(cmd);
  i2c_master_cmd_begin(I2C_NUM_0, cmd, 1000 / portTICK_RATE_MS);
  i2c_cmd_link_delete(cmd);

//  Wire.requestFrom(device_addr,1);
//  ret = Wire.read(); // receive DATA
//  
  return ret;
}
