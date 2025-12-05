/*
  # FreeRTOS Semáforo desde una ISR
  Este sketch muestra cómo una Rutina de Servicio de Interrupción (ISR) puede "dar" un semáforo para desbloquear una tarea.

  ## Funcionalidad
  Se configura un temporizador de hardware para que genere una interrupción cada 3 segundos. La ISR asociada a este temporizador (`onTimer`) libera un semáforo binario. Una tarea, `TakerTask`, espera bloqueada hasta que este semáforo es liberado por la ISR.
  
  Este es un patrón de diseño muy común para manejar eventos de hardware (como la llegada de datos por UART, un pin que cambia de estado, etc.) de manera eficiente sin consumir CPU en esperas activas (polling). La tarea "duerme" hasta que la interrupción le avisa que hay trabajo por hacer.

  ## Variaciones de la funcionalidad
  - **Cambiar la fuente de la interrupción:** En lugar de un temporizador, puedes usar `attachInterrupt()` para asociar la ISR a un pin de GPIO. Esto permitiría, por ejemplo, que `TakerTask` se ejecute cada vez que se presiona un botón físico.
    ```c++
    // En setup():
    pinMode(BUTTON_PIN, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(BUTTON_PIN), &myISR, FALLING);
    // La ISR 'myISR' haría lo mismo que 'onTimer'.
    ```
  - **Pasar datos desde la ISR:** Aunque no se puede pasar directamente, la ISR puede escribir en una cola (Queue) y luego dar un semáforo. La tarea, al despertarse, sabría que hay nuevos datos en la cola listos para ser procesados.
*/
#include <M5Stack.h>

SemaphoreHandle_t xSemaphore = NULL;
hw_timer_t *timer = NULL;

// Rutina de Servicio de Interrupción (ISR) que se ejecuta cuando el temporizador se dispara.
// Debe ser rápida y no puede usar funciones que puedan bloquearse (como Serial.println).
void IRAM_ATTR onTimer() {
  // Variable para notificar si una tarea de mayor prioridad ha sido desbloqueada.
  BaseType_t xHigherPriorityTaskWoken = pdFALSE;

  // Libera el semáforo DESDE UNA ISR. Es crucial usar la versión 'FromISR'.
  // El segundo parámetro recibirá un valor si la liberación del semáforo desbloquea una tarea de mayor prioridad que la actual.
  xSemaphoreGiveFromISR(xSemaphore, &xHigherPriorityTaskWoken);

  // Si una tarea de mayor prioridad fue desbloqueada, forzamos un cambio de contexto inmediato.
  // Esto asegura que la tarea urgente se ejecute tan pronto como la ISR termine.
  if (xHigherPriorityTaskWoken) {
    portYIELD_FROM_ISR();
  }
}

// Tarea que espera (toma) el semáforo.
void TakerTask(void *pvParameters) {
  for (;;) {
    Serial.println("TakerTask: Esperando el semáforo desde la ISR.");
    // La tarea se bloquea aquí, sin consumir CPU, hasta que la ISR le da el semáforo.
    if (xSemaphoreTake(xSemaphore, portMAX_DELAY) == pdTRUE) {
      Serial.println("TakerTask: ¡Semáforo tomado! Procesando el evento...");
      // Aquí iría el código que maneja el evento que causó la interrupción.
    }
  }
}

void setup() {
  M5.begin();
  Serial.begin(115200);

  M5.Lcd.println("FreeRTOS ISR Semaphore Example");

  // Crea el semáforo binario.
  xSemaphore = xSemaphoreCreateBinary();

  if (xSemaphore != NULL) {
    // Crea la tarea que esperará la interrupción.
    xTaskCreate(TakerTask, "Taker", 2048, NULL, 2, NULL); // Prioridad 2, más alta que la de sistema.

    // Configura un temporizador de hardware (Timer 0 de 4).
    timer = timerBegin(0, 80, true); // Timer 0, prescaler 80 (para 1MHz), cuenta hacia arriba.
    timerAttachInterrupt(timer, &onTimer, true); // Asocia la ISR 'onTimer' al temporizador.
    timerAlarmWrite(timer, 3000000, true); // Dispara la alarma cada 3,000,000 de ciclos (3 segundos). 'true' para auto-recarga.
    timerAlarmEnable(timer); // Habilita la alarma del temporizador.

  } else {
    Serial.println("Fallo al crear el semáforo");
  }
}

void loop() {
  // No se utiliza.
}
