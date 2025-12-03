
/****************************************************************/
/*Ejercicio 4: Sincronizacion entre ISR y tarea                 */
/*  Se interrumpe el micro al pulsar el boton A 				*/
/****************************************************************/
#define PIN_PULSADOR_A 39

//Crear el manejador para el semáforo
SemaphoreHandle_t S1;


// Rutina de interrupción, llamada cuando se presiona el pulsador
void IRAM_ATTR pulsadorA_ISR() {
  BaseType_t xHigherPriorityTaskWoken = pdFALSE;
  // dar el semáforo desde el ISR para que quede libre para la tarea pulsador
  xSemaphoreGiveFromISR(S1, &xHigherPriorityTaskWoken);
}

void task_pulsador(void* arg) {

  while (1) {
    // Esperar que se active el semaforo 
	// y escribe en el monitor serie
	// "Boton A pulsado"
    if (xSemaphoreTake(S1, portMAX_DELAY) == pdTRUE) {
      Serial.println("Boton A pulsado");
    }
  }
}


void setup() {
  Serial.begin(115200);
  Serial.println("Sincronizacion ISR --> tarea: ");

  // Configurar interrupcion del pulsador A
  pinMode(PIN_PULSADOR_A, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(PIN_PULSADOR_A), pulsadorA_ISR, FALLING);

  // Crear semaforo binario
  S1 = xSemaphoreCreateBinary();

  // Crear la tarea task_pulsador
  xTaskCreate(task_pulsador, "task_pulsador", 2048, NULL, 1, NULL);
}

void loop() {}
