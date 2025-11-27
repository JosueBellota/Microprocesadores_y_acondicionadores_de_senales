/* Ejercicio 6: Seccion critica vinculada a tareas
*/
#define PIN_PULSADOR_A 39
#define PIN_PULSADOR_B 38

//declarar el manejador para el semáforo binario como variable global
SemaphoreHandle_t xSemaphore_A = NULL;
SemaphoreHandle_t xSemaphore_B = NULL;

//declarar el manejador para el semáforo mutex como variable global


static int cEvents; 
int const a_end = 32767;
int const b_end = 199;
volatile int incr_2 = 0;

// Rutina de interrupción, llamada cuando se presiona el pulsador
void IRAM_ATTR pulsadorA_ISR() {
    // da el semáforo para que quede libre para la tarea pulsador
   xSemaphoreGiveFromISR(xSemaphore_A, NULL);
}
void IRAM_ATTR pulsadorB_ISR() {
    // da el semáforo para que quede libre para la tarea pulsador
   xSemaphoreGiveFromISR(xSemaphore_B, NULL);
}

void task_pulsador_A(void* arg) {

   while(1) {
      int incr = 1;
      int cuenta;
      // Espero por la notificación de la ISR
      if(xSemaphoreTake(xSemaphore_A,portMAX_DELAY) == pdTRUE) {
            cuenta = cEvents;
            Delay_c(a_end,b_end);
            cuenta += incr;
            cEvents = cuenta;
            Serial.print("Pulsador A presionado. cEvents = ");
            Serial.println(cEvents);
      }
   }
}
void task_pulsador_B(void* arg) {

   while(1) {
      // Espero por la notificación de la ISR
      if(xSemaphoreTake(xSemaphore_B,portMAX_DELAY) == pdTRUE) {
            cEvents ++;
            Serial.print("Pulsador B presionado. cEvents = ");
            Serial.println(cEvents);
      }
   }
}

void setup() {
  Serial.begin(115200);
  Serial.println("Monitoring interrupts: ");
  pinMode(PIN_PULSADOR_A, INPUT_PULLUP);
  pinMode(PIN_PULSADOR_B, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(PIN_PULSADOR_A), pulsadorA_ISR, RISING);
  attachInterrupt(digitalPinToInterrupt(PIN_PULSADOR_B), pulsadorB_ISR, RISING);

   // se crea el semáforo binario
   xSemaphore_A = xSemaphoreCreateBinary();
   xSemaphore_B = xSemaphoreCreateBinary();
   
   //Se crea el Mutex
    
   // creo la tarea task_pulsador
   xTaskCreate(task_pulsador_A, "task_pulsador", 2048, NULL, 4, NULL);
   xTaskCreate(task_pulsador_B, "task_pulsador", 2048, NULL, 4, NULL);
}

void Delay_c(int const a_end, int const b_end){
    int a, b;
      for (a = 1; a <= a_end; a++){
        for (b = 1; b <= b_end; b++)
          {    
           incr_2++;
           }
        };
}
void loop() {}
