/*
  # I2C Slave Receiver
  Este sketch demuestra el uso de un dispositivo como esclavo I2C que recibe datos de un maestro.

  ## Funcionalidad
  El código inicializa el dispositivo como un esclavo I2C con una dirección específica (8). Registra una función (`receiveEvent`) que se ejecutará automáticamente cada vez que reciba datos del maestro. Esta función lee los bytes recibidos y los imprime en el monitor serie.

  ## Variaciones de la funcionalidad
  - **Cambiar la dirección del esclavo:** Para que el dispositivo escuche en una dirección diferente, cambia el número en `Wire.begin(8)`. Esta dirección debe coincidir con la que el maestro usa para transmitir.
  - **Procesar los datos recibidos:** En lugar de simplemente imprimir los datos, puedes usarlos para controlar actuadores (LEDs, motores), actualizar variables, o realizar cualquier otra acción. La lógica de procesamiento iría dentro de la función `receiveEvent`.
  - **Manejar diferentes tipos de datos:** El ejemplo asume que recibe una cadena seguida de un entero. Si el maestro envía un formato de datos diferente, necesitarás ajustar el código en `receiveEvent` para leer y decodificar los bytes correctamente. Por ejemplo, si solo recibes números, podrías leerlos todos en un bucle `while(Wire.available())`.
*/
#include <Wire.h>

void setup() {
  // Inicializa la comunicación I2C como esclavo en la dirección 8.
  // Cada dispositivo en el bus I2C debe tener una dirección única.
  Wire.begin(8);                
  
  // Registra la función 'receiveEvent' para que se llame cuando el esclavo reciba datos.
  Wire.onReceive(receiveEvent); 
  
  // Inicializa la comunicación serie para mostrar los datos recibidos.
  Serial.begin(9600);           
}

void loop() {
  // El loop puede estar vacío o realizar otras tareas.
  // La recepción de datos I2C es manejada por interrupciones.
  delay(100);
}

// Esta función se ejecuta cada vez que se reciben datos del maestro.
// El parámetro 'howMany' indica el número de bytes recibidos.
void receiveEvent(int howMany) {
  // Itera a través de todos los bytes recibidos excepto el último.
  while (1 < Wire.available()) { 
    // Lee un byte como un carácter.
    char c = Wire.read(); 
    // Imprime el carácter en el monitor serie.
    Serial.print(c);         
  }
  // Lee el último byte como un entero.
  int x = Wire.read();    
  // Imprime el entero en una nueva línea.
  Serial.println(x);         
}