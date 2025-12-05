/*
  # FreeRTOS Mutex (Exclusión Mutua)
  Este sketch demuestra cómo usar un 'mutex' para proteger un recurso compartido del acceso simultáneo por parte de múltiples tareas.

  ## Funcionalidad
  Hay una variable global, `sharedResource`. Dos tareas, `Task1` y `Task2`, intentan acceder y modificar esta variable. Para evitar condiciones de carrera (donde una tarea podría leer el valor mientras otra lo está modificando, llevando a resultados impredecibles), se usa un mutex.

  Antes de acceder a `sharedResource`, cada tarea debe "tomar" el mutex. Si el mutex ya está tomado por otra tarea, la tarea que intenta tomarlo se bloqueará hasta que sea liberado. Una vez que la tarea ha terminado de usar el recurso, debe "dar" (liberar) el mutex para que otras tareas puedan acceder a él. Esto garantiza que solo una tarea a la vez pueda modificar `sharedResource`.

  ## Variaciones de la funcionalidad
  - **Proteger diferentes recursos:** El mutex puede proteger cualquier tipo de recurso compartido: una variable, un periférico como la pantalla LCD, un bus de comunicación como el I2C, o un archivo en una tarjeta SD. Simplemente encierra el código que accede al recurso entre `xSemaphoreTake(xMutex, ...)` y `xSemaphoreGive(xMutex)`.
  - **Mutex Recursivo:** Si una tarea que ya posee un mutex necesita tomarlo de nuevo (por ejemplo, si una función llama a otra función y ambas toman el mismo mutex), se debe usar un 'mutex recursivo' (`xSemaphoreCreateRecursiveMutex()`).
  - **Tiempo de espera:** Al igual que con los semáforos, puedes especificar un tiempo de espera en `xSemaphoreTake`. Si la tarea no consigue el mutex en ese tiempo, puede realizar otra acción en lugar de quedarse bloqueada indefinidamente.
*/
#include <M5Stack.h>

// Handle para el mutex. Los mutex son en realidad un tipo especial de semáforo.
SemaphoreHandle_t xMutex = NULL;
// El recurso compartido que queremos proteger.
int sharedResource = 0;

// Tarea que accede al recurso compartido.
void Task1(void *pvParameters) {
  for (;;) {
    Serial.println("Task1: Intentando tomar el mutex.");
    // Intenta tomar el mutex. Si está ocupado, la tarea se bloqueará aquí.
    if (xSemaphoreTake(xMutex, portMAX_DELAY) == pdTRUE) {
      // --- INICIO DE LA SECCIÓN CRÍTICA ---
      // Ahora tenemos el control exclusivo del recurso.
      Serial.println("Task1: Mutex tomado. Accediendo al recurso.");
      sharedResource++;
      Serial.print("Task1: El recurso compartido ahora es ");
      Serial.println(sharedResource);

      // Libera el mutex para que otras tareas puedan usarlo.
      xSemaphoreGive(xMutex);
      // --- FIN DE LA SECCIÓN CRÍTICA ---
    }
    vTaskDelay(1000 / portTICK_PERIOD_MS);
  }
}

// Otra tarea que accede al mismo recurso.
void Task2(void *pvParameters) {
  for (;;) {
    Serial.println("Task2: Intentando tomar el mutex.");
    if (xSemaphoreTake(xMutex, portMAX_DELAY) == pdTRUE) {
      // --- INICIO DE LA SECCIÓN CRÍTICA ---
      Serial.println("Task2: Mutex tomado. Accediendo al recurso.");
      sharedResource++;
      Serial.print("Task2: El recurso compartido ahora es ");
      Serial.println(sharedResource);

      // Es crucial liberar el mutex después de usar el recurso.
      xSemaphoreGive(xMutex);
      // --- FIN DE LA SECCIÓN CRÍTICA ---
    }
    vTaskDelay(1500 / portTICK_PERIOD_MS);
  }
}

void setup() {
  M5.begin();
  Serial.begin(115200);

  M5.Lcd.println("FreeRTOS Mutex Example");

  // Crea un mutex. Comienza "disponible".
  xMutex = xSemaphoreCreateMutex();

  if (xMutex != NULL) {
    // Crea las tareas que competirán por el recurso.
    xTaskCreate(Task1, "Task1", 2048, NULL, 1, NULL);
    xTaskCreate(Task2, "Task2", 2048, NULL, 1, NULL);
  } else {
    Serial.println("Fallo al crear el mutex");
  }
}

void loop() {
  // No se utiliza.
}
