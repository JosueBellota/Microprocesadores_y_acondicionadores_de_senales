/*
  # FreeRTOS Colas (Queues)
  Este sketch demuestra cómo usar una cola (queue) para enviar datos de forma segura entre dos tareas.

  ## Funcionalidad
  Se crean dos tareas: `ProducerTask` y `ConsumerTask`.
  - `ProducerTask` genera un número entero cada 2 segundos y lo envía a una cola.
  - `ConsumerTask` espera a que lleguen datos a la cola. Cuando recibe un dato, lo imprime en el monitor serie.

  Las colas son el método principal y más seguro para pasar datos entre tareas o entre una ISR y una tarea. FreeRTOS se encarga de la sincronización, asegurando que los datos no se corrompan, incluso si el productor y el consumidor tienen prioridades diferentes. Los datos se copian en la cola, no se pasan por referencia.

  ## Variaciones de la funcionalidad
  - **Cambiar el tipo de datos:** La cola puede contener cualquier tipo de dato, incluyendo estructuras (`struct`). Simplemente cambia el `sizeof(int)` en `xQueueCreate` por `sizeof(MyStruct)` y usa variables de tipo `MyStruct` para enviar y recibir.
    ```c++
    // struct MyData { int id; float value; };
    // xQueue = xQueueCreate(10, sizeof(MyData));
    // MyData data_to_send;
    // xQueueSend(xQueue, &data_to_send, ...);
    ```
  - **Cambiar el tamaño de la cola:** El primer parámetro de `xQueueCreate` (actualmente 5) define cuántos elementos puede almacenar la cola. Si el productor intenta enviar a una cola llena, se bloqueará hasta que haya espacio.
  - **Envío desde ISR:** Puedes enviar datos a una cola desde una ISR usando `xQueueSendFromISR()`. Este es un patrón muy potente para procesar datos de periféricos. La ISR captura el dato y lo pone en la cola, y una tarea de menor prioridad lo procesa tranquilamente.
*/
#include <M5Stack.h>

// Handle para la cola.
QueueHandle_t xQueue = NULL;

// Tarea que envía datos a la cola (Productor).
void ProducerTask(void *pvParameters) {
  int dataToSend = 0;
  for (;;) {
    Serial.print("ProducerTask: Enviando dato a la cola: ");
    Serial.println(dataToSend);
    
    // Envía el valor de 'dataToSend' a la cola.
    // La tarea se bloqueará si la cola está llena, hasta que haya espacio disponible.
    xQueueSend(xQueue, &dataToSend, portMAX_DELAY);
    
    dataToSend++;
    vTaskDelay(2000 / portTICK_PERIOD_MS);
  }
}

// Tarea que recibe datos de la cola (Consumidor).
void ConsumerTask(void *pvParameters) {
  int receivedData;
  for (;;) {
    Serial.println("ConsumerTask: Esperando datos de la cola...");
    // Espera a recibir un dato de la cola.
    // La tarea se bloqueará aquí si la cola está vacía, hasta que llegue un dato.
    if (xQueueReceive(xQueue, &receivedData, portMAX_DELAY) == pdTRUE) {
      Serial.print("ConsumerTask: Dato recibido de la cola: ");
      Serial.println(receivedData);
    }
  }
}

void setup() {
  M5.begin();
  Serial.begin(115200);

  M5.Lcd.println("FreeRTOS Queue Example");

  // Crea una cola.
  // 1er parámetro: Número máximo de elementos que puede almacenar (longitud).
  // 2do parámetro: Tamaño de cada elemento en bytes.
  xQueue = xQueueCreate(5, sizeof(int)); 

  if (xQueue != NULL) {
    // Crea las tareas productora y consumidora.
    xTaskCreate(ProducerTask, "Producer", 2048, NULL, 1, NULL);
    xTaskCreate(ConsumerTask, "Consumer", 2048, NULL, 2, NULL); // Mayor prioridad para procesar datos rápido.
  } else {
    Serial.println("Fallo al crear la cola");
  }
}

void loop() {
  // No se utiliza.
}
