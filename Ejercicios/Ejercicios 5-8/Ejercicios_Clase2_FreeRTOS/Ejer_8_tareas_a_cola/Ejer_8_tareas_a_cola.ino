/* Ejercicio 8: Enviar datos de 3 tareas a una tarea receptora mediante una cola*/
#include <Arduino.h>

#define TAM_COLA 20 /*20 mensajes*/

// Estructura para envio de datos
typedef struct{
  uint8_t id_sender;
  uint8_t data;
}type_msg;

// Declarar manejador de la cola
QueueHandle_t cola_datos;


int msg1 = 0;
void escribe1(void *pvParameter)
{
    type_msg my_msg;
    my_msg.id_sender = 1;
    uint8_t cont_t1 = 0;
    while(1) {
    cont_t1++;
    my_msg.data = cont_t1;
    printf("Ejecuta Tarea escribe1\n");
    // Enviar datos a la cola
    xQueueSend(cola_datos, &my_msg, portMAX_DELAY);

    vTaskDelay(2000 / portTICK_RATE_MS);
    }
}
void escribe2(void *pvParameter)
{
    type_msg my_msg;
    my_msg.id_sender = 2;
    uint8_t cont_t2 = 0;
    while(1) {
    cont_t2++;
    my_msg.data = cont_t2;
    printf("Ejecuta Tarea escribe2\n");
    // Enviar datos a la cola
    xQueueSend(cola_datos, &my_msg, portMAX_DELAY);

     vTaskDelay(2000 / portTICK_RATE_MS);
 }
}

void escribe3(void *pvParameter)
{
  type_msg my_msg;
    my_msg.id_sender = 3;
    uint8_t cont_t3 = 0;
    while(1) {
    cont_t3++;
    my_msg.data = cont_t3;
    printf("Ejecuta Tarea escribe3\n");
    // Enviar datos a la cola
    xQueueSend(cola_datos, &my_msg, portMAX_DELAY);

    vTaskDelay(2000 / portTICK_RATE_MS);
    }
}

void lee1(void *pvParameter)
{
   type_msg rx_msg;
   while(1) { 
      // Leer datos de la cola
      if (xQueueReceive(cola_datos, &rx_msg, portMAX_DELAY)) {
        printf("Tarea receptora: Recibido de la tarea %d el dato: %d\n", rx_msg.id_sender, rx_msg.data);
      }
   }
}

void setup() {
    Serial.begin(115200);
    // Crear cola
    cola_datos = xQueueCreate(TAM_COLA, sizeof(type_msg));

    // Crear tareas
    xTaskCreate(&lee1,     "lee1",     1024*10, NULL, 4, NULL);
    xTaskCreate(&escribe1, "escribe1", 1024, NULL, 1, NULL);
    xTaskCreate(&escribe2, "escribe2", 1024, NULL, 1, NULL);
    xTaskCreate(&escribe3, "escribe3", 1024, NULL, 1, NULL);
}

void loop() {}