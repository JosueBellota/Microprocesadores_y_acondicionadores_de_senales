/*
  # FreeRTOS Semáforo Binario
  Este sketch demuestra el uso de un semáforo binario para sincronizar dos tareas en FreeRTOS.

  ## Funcionalidad
  Se crean dos tareas: `GiverTask` y `TakerTask`.
  - `GiverTask` libera (da) un semáforo cada 2 segundos.
  - `TakerTask` espera (toma) indefinidamente hasta que el semáforo esté disponible.
  
  Esto crea una sincronización donde `TakerTask` solo puede proceder después de que `GiverTask` le dé permiso a través del semáforo. Un semáforo binario es como una bandera que solo puede estar arriba o abajo.

  ## Variaciones de la funcionalidad
  - **Cambiar el tiempo de espera:** En `xSemaphoreTake(xSemaphore, portMAX_DELAY)`, el segundo parámetro es el tiempo máximo que la tarea esperará. `portMAX_DELAY` significa esperar indefinidamente. Puedes usar un valor en 'ticks' para que la tarea se rinda si no obtiene el semáforo a tiempo. Por ejemplo, `xSemaphoreTake(xSemaphore, 1000 / portTICK_PERIOD_MS)` esperaría 1 segundo.
  - **Sincronización con eventos externos:** En lugar de que una tarea libere el semáforo en un intervalo de tiempo, podrías liberarlo en una interrupción (ver ejemplo `03_ISR_Semaphore`), como al presionar un botón.
  - **Sección crítica simple:** Se puede usar un semáforo para proteger una sección corta de código y asegurar que solo una tarea acceda a ella a la vez, aunque para esto es más común y seguro usar un 'mutex' (ver ejemplo `04_Mutex`).
*/
#include <M5Stack.h>

// Handle para el semáforo.
SemaphoreHandle_t xSemaphore = NULL;

// Tarea que libera el semáforo.
void GiverTask(void *pvParameters) {
  for (;;) {
    // Espera 2 segundos.
    vTaskDelay(2000 / portTICK_PERIOD_MS);
    Serial.println("GiverTask: Dando el semáforo.");
    // Libera el semáforo, permitiendo que una tarea que lo espera continúe.
    xSemaphoreGive(xSemaphore);
  }
}

// Tarea que toma el semáforo.
void TakerTask(void *pvParameters) {
  for (;;) {
    Serial.println("TakerTask: Esperando por el semáforo.");
    // Intenta tomar el semáforo. La tarea se bloqueará aquí hasta que el semáforo esté disponible.
    // portMAX_DELAY hace que espere indefinidamente.
    if (xSemaphoreTake(xSemaphore, portMAX_DELAY) == pdTRUE) {
      // Si se pudo tomar el semáforo, pdTRUE es devuelto.
      Serial.println("TakerTask: ¡Semáforo tomado!");
    }
  }
}

void setup() {
  M5.begin();
  Serial.begin(115200);

  M5.Lcd.println("FreeRTOS Binary Semaphore Example");

  // Crea un semáforo binario. Comienza "tomado" (vacío).
  xSemaphore = xSemaphoreCreateBinary();

  if (xSemaphore != NULL) {
    // Crea las dos tareas.
    xTaskCreate(GiverTask, "Giver", 1024, NULL, 1, NULL);
    xTaskCreate(TakerTask, "Taker", 1024, NULL, 1, NULL);
  } else {
    Serial.println("Fallo al crear el semáforo");
  }
}

void loop() {
  // No se utiliza.
}
