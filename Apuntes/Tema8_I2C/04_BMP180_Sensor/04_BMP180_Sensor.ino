/*
  # BMP180 Digital Pressure Sensor Example

  Este sketch demuestra cómo leer datos (temperatura, presión y altitud) de un sensor BMP180
  conectado a un M5Stack-Core-ESP32 a través de la interfaz I2C.

  ## Funcionalidad
  El código inicializa la comunicación I2C y el sensor BMP180. En la función `setup()`,
  realiza una lectura inicial de temperatura, presión y altitud, y muestra estos valores
  tanto en el Monitor Serie como en la pantalla LCD del M5Stack.
  En el `loop()`, las lecturas se actualizan cada 5 segundos y se vuelven a mostrar.

  ## Hardware (M5Stack-Core-ESP32 y BMP180)
  El sensor BMP180 se comunica vía I2C. En el M5Stack-Core-ESP32, los pines I2C estándar
  son GPIO21 (SDA) y GPIO22 (SCL).
  Asegúrate de conectar el sensor BMP180 de la siguiente manera:
  - VCC del BMP180 a 3.3V del M5Stack
  - GND del BMP180 a GND del M5Stack
  - SDA del BMP180 a GPIO21 del M5Stack
  - SCL del BMP180 a GPIO22 del M5Stack

  ## Librerías Necesarias
  Para este ejemplo, necesitas instalar la librería "Adafruit BMP085 Library"
  (compatible con BMP180) a través del Gestor de Librerías del IDE de Arduino.

  ## Variaciones de la funcionalidad
  - **Cambiar la frecuencia de lectura:** Modifica el valor en `delay(5000)` en el `loop()`
    para leer el sensor más o menos frecuentemente.
  - **Mostrar datos de forma diferente:** Puedes cambiar la forma en que los datos se muestran
    en la pantalla LCD o en el Monitor Serie, o incluso enviarlos a través de Wi-Fi.
  - **Calibración de altitud:** La función `bmp.readAltitude(101500)` utiliza un valor de presión
    a nivel del mar para calcular una altitud más precisa. Deberías reemplazar `101500`
    por la presión atmosférica actual a nivel del mar en tu ubicación para obtener lecturas
    de altitud más exactas. Este valor puede variar con las condiciones meteorológicas.
    Puedes obtener la presión actual a nivel del mar de estaciones meteorológicas locales.
  - **Manejo de errores:** El código incluye una comprobación básica en `setup()` (`if (!bmp.begin())`).
    Puedes expandir esto con más lógica de reintento o notificación de errores.
*/
#include <M5Stack.h>
#include <Wire.h>
// Incluye la librería Adafruit BMP085, que es compatible con el BMP180.
// Asegúrate de instalarla desde el Gestor de Librerías del IDE de Arduino.
#include <Adafruit_BMP085.h>

// Crea un objeto para el sensor BMP180.
Adafruit_BMP085 bmp;

void setup() {
  // Inicializa el M5Stack (pantalla, SD, etc.).
  M5.begin();
  // Inicializa la comunicación serial para depuración y visualización de datos.
  Serial.begin(115200);

  M5.Lcd.println("BMP180 Sensor Test");

  // Intenta inicializar el sensor BMP180.
  // Si falla (por ejemplo, el sensor no está conectado o hay un problema en el cableado I2C),
  // imprime un mensaje de error y entra en un bucle infinito.
  if (!bmp.begin()) {
    Serial.println("No se pudo encontrar un sensor BMP180 válido, ¡verifica el cableado!");
    M5.Lcd.println("Error: BMP180 not found!");
    while (1) {}
  }

  // Muestra las primeras lecturas en la pantalla LCD.
  M5.Lcd.print("Temp: ");
  M5.Lcd.print(bmp.readTemperature());
  M5.Lcd.println(" *C");

  M5.Lcd.print("Pres: ");
  M5.Lcd.print(bmp.readPressure());
  M5.Lcd.println(" Pa");

  // Calcula la altitud asumiendo una atmósfera 'estándar' y presión a nivel del mar por defecto.
  M5.Lcd.print("Alt: ");
  M5.Lcd.print(bmp.readAltitude());
  M5.Lcd.println(" metros");

  Serial.println("BMP180 Sensor inicializado y primeras lecturas mostradas en LCD.");
}

void loop() {
  // Retardo para no leer el sensor constantemente. Esto se puede ajustar.
  delay(5000); // Espera 5 segundos para la siguiente lectura.

  // Realiza las lecturas del sensor.
  float temperature = bmp.readTemperature();
  long pressure = bmp.readPressure();
  float altitude = bmp.readAltitude(); // Altitud calculada con presión a nivel del mar por defecto.

  // Limpia el área de la pantalla donde se muestran los datos para evitar superposiciones.
  M5.Lcd.fillRect(0, 50, 320, 100, BLACK);
  M5.Lcd.setCursor(0, 50); // Establece el cursor para las nuevas lecturas.

  // Muestra las lecturas actualizadas en la pantalla LCD.
  M5.Lcd.print("Temp: ");
  M5.Lcd.print(temperature);
  M5.Lcd.println(" *C");

  M5.Lcd.print("Pres: ");
  M5.Lcd.print(pressure);
  M5.Lcd.println(" Pa");
  
  M5.Lcd.print("Alt: ");
  M5.Lcd.print(altitude);
  M5.Lcd.println(" m");

  // También imprime las lecturas en el Monitor Serie.
  Serial.print("Temperature = ");
  Serial.print(temperature);
  Serial.println(" *C");

  Serial.print("Pressure = ");
  Serial.print(pressure);
  Serial.println(" Pa");

  Serial.print("Altitude = ");
  Serial.print(altitude);
  Serial.println(" meters");
  Serial.println("---");
}
