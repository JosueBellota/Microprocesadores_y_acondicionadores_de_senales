/* Ejercicio 7: Comunicación UART por eventos*/

#include <M5Stack.h>
#include "driver/uart.h"
#include "esp_intr_alloc.h"
#include "soc/uart_reg.h"


#define TX0_pin 1
#define RX0_pin 3

#define BUF_SIZE (1024)


#define TAM_COLA 10 /*num mensajes*/
#define TAM_MSG 5   /*num caracteres por mensaje*/

// 1. Declaracion manejador de la cola //////////////////////////////////
xQueueHandle cola_events;




void setup() {
  M5.begin(1, 0, 0, 1);
  M5.Lcd.setTextSize(3);
  M5.Lcd.printf("\n\n");
  M5.Lcd.printf("  Comunicacion\r\n");
  M5.Lcd.printf("\r\n");
  M5.Lcd.printf("   Evento ->tarea\r\n");
  M5.Lcd.printf("\r\n");
  M5.Lcd.printf("    con cola\r\n");


  // Deshabilita la UART0 para configurarla con funciones idf
  Serial.end();

  // 2. Crear  cola de eventos/////////////////
  cola_events = xQueueCreate(TAM_COLA, TAM_MSG);


  // 3. Configurar UART0
  uart_config_t uart_config = {
    .baud_rate = 115200,
    .data_bits = UART_DATA_8_BITS,
    .parity = UART_PARITY_DISABLE,
    .stop_bits = UART_STOP_BITS_1,
    .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
    .rx_flow_ctrl_thresh = 120,
    .use_ref_tick = false
  };

  uart_param_config(UART_NUM_0, &uart_config);
  uart_set_pin(UART_NUM_0, TX0_pin, RX0_pin, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
  // Instalar el driver y asociarlo a la cola
  //uart_driver_install(uart_port_t uart_num, int rx_buffer_size, int tx_buffer_size, int queue_size, QueueHandle_t *uart_queue, int intr_alloc_flags)
  uart_driver_install(UART_NUM_0, BUF_SIZE * 2, BUF_SIZE * 2, 20, &cola_events, 0); //envia los eventos a la cola de eventos

  // 4. Creacion de tarea
  xTaskCreate(&lee_cola, "lee_cola", 1024, NULL, 4, NULL);
}

void lee_cola(void *pvParameter) {
  uart_event_t event;
  int i;
  char Rx[TAM_COLA];
  while (1) {
    // Si se produce el evento de escritura en el monitor serie
    if (xQueueReceive(cola_events, (void *)&event, (TickType_t)portMAX_DELAY) == pdTRUE)  //
    {
      // 4.a Leer del monitor serie
      uart_read_bytes(UART_NUM_0, Rx, event.size, portMAX_DELAY);

      // 4.b. escribir los datos en el monitor serie
      uart_write_bytes(UART_NUM_0, (const char *)Rx, event.size);

      // Limpiamos el buffer de la UART
      uart_flush(UART_NUM_0);
    }
  }
}

void loop(){}