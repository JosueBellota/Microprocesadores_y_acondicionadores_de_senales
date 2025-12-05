/*
  # FreeRTOS Tarea Básica
  Este sketch demuestra cómo crear y ejecutar una tarea simple usando FreeRTOS en un M5Stack.

  ## Funcionalidad
  El código define una función `myTask` que imprime un mensaje en el monitor serie cada segundo. En la función `setup`, se crea esta tarea usando `xTaskCreate`. Una vez creada, el planificador de FreeRTOS se encarga de ejecutarla. El `loop` principal de Arduino queda vacío, ya que la lógica de la aplicación se traslada a las tareas.

  ## Variaciones de la funcionalidad
  - **Cambiar la frecuencia de la tarea:** Modifica el valor en `vTaskDelay(1000 / portTICK_PERIOD_MS)`. El valor está en milisegundos. Por ejemplo, `vTaskDelay(500 / portTICK_PERIOD_MS)` haría que la tarea se ejecute cada medio segundo.
  - **Realizar diferentes acciones:** Reemplaza `Serial.println(...)` con cualquier código que necesites ejecutar, como leer un sensor, actualizar la pantalla LCD o controlar un actuador.
  - **Cambiar la prioridad:** El parámetro de prioridad en `xTaskCreate` (actualmente 1) determina qué tarea se ejecuta si varias están listas. Un número más alto significa una prioridad mayor. `xTaskCreate(myTask, "MyTask", 1024, NULL, 2, NULL);`
  - **Pasar parámetros a la tarea:** Puedes pasar un puntero a una variable o estructura a la tarea a través del parámetro `pvParameters`. Esto es útil para configurar la tarea en el momento de la creación.
*/
#include <M5Stack.h>

// Esta es la función que se ejecutará como una tarea independiente.
// Toda tarea en FreeRTOS debe tener un bucle infinito y un retardo para ceder el control.
void myTask(void *pvParameters) {
  // El parámetro pvParameters se usa para pasar datos a la tarea al crearla. En este ejemplo no se usa.
  (void) pvParameters;

  for (;;) { // Bucle infinito que mantiene la tarea en ejecución.
    Serial.println("Hello from myTask!");
    // Pausa la tarea por 1000 milisegundos (1 segundo).
    // vTaskDelay es la forma correcta de crear retardos en FreeRTOS, ya que permite que otras tareas se ejecuten.
    vTaskDelay(1000 / portTICK_PERIOD_MS); 
  }
}

void setup() {
  M5.begin();
  Serial.begin(115200);

  M5.Lcd.println("Creating a basic FreeRTOS task.");

  // Crea una nueva tarea.
  xTaskCreate(
    myTask,           // 1. Puntero a la función de la tarea.
    "MyTask",         // 2. Nombre de la tarea (útil para depuración).
    1024,             // 3. Tamaño de la pila en palabras. Si la tarea es compleja, puede necesitar más.
    NULL,             // 4. Parámetro para pasar a la tarea (en este caso, ninguno).
    1,                // 5. Prioridad de la tarea (0 es la más baja).
    NULL              // 6. Handle de la tarea (para referenciarla después, si es necesario).
  );
}

void loop() {
  // En muchas aplicaciones con FreeRTOS, el loop principal de Arduino se deja vacío.
  // El planificador del sistema operativo se encarga de ejecutar las tareas.
}
