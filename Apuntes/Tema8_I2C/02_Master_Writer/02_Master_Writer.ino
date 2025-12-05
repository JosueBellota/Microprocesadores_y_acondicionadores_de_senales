/*
  # I2C Master Writer
  Este sketch demuestra el uso de un dispositivo como maestro I2C que envía datos a un esclavo.

  ## Funcionalidad
  El código inicializa la comunicación I2C y serie. En el bucle principal, envía repetidamente una cadena de texto y un valor numérico que se va incrementando a un dispositivo esclavo con la dirección 8.

  ## Variaciones de la funcionalidad
  - **Cambiar la dirección del esclavo:** Para enviar datos a un dispositivo con una dirección diferente, cambia el número en `Wire.beginTransmission(8)`. Por ejemplo, para un dispositivo en la dirección 0x27, usarías `Wire.beginTransmission(0x27)`.
  - **Enviar diferentes tipos de datos:** Puedes enviar cualquier tipo de dato que `Wire.write()` acepte (cadenas, arrays de bytes, enteros, etc.).
    - `Wire.write("hola");`
    - `Wire.write(123);`
    - `byte data[] = {1, 2, 3}; Wire.write(data, 3);`
  - **Controlar cuándo se envían los datos:** En lugar de enviar datos continuamente, podrías enviarlos en respuesta a un evento, como presionar un botón o recibir un comando serie.
*/
#include <Wire.h>

void setup() {
  // Inicializa la comunicación I2C como maestro. 
  // No se especifica una dirección, ya que el maestro inicia la comunicación.
  Wire.begin(); 
  // Inicializa la comunicación serie para depuración (opcional).
  Serial.begin(9600);
}

// Variable para llevar la cuenta de los datos enviados.
byte x = 0;

void loop() {
  // Inicia una transmisión al dispositivo esclavo con la dirección 8.
  // ¡Asegúrate de que esta dirección coincida con la del dispositivo esclavo!
  Wire.beginTransmission(8); 
  
  // Envía datos al esclavo. Se pueden llamar a Wire.write() varias veces.
  // Los datos se almacenan en un búfer temporal.
  Wire.write("x is ");        // Envía una cadena de 5 bytes.
  Wire.write(x);              // Envía el valor actual de x como un byte.
  
  // Finaliza la transmisión y envía los datos del búfer al esclavo.
  Wire.endTransmission();    

  // Incrementa el valor para el próximo envío.
  x++;
  // Espera medio segundo antes de volver a enviar.
  delay(500);
}