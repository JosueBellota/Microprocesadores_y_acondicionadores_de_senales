/* CLASE UART Ejem_3*/

#include <M5Stack.h>
#include "driver/uart.h"

#define TX0_pin  1
#define RX0_pin  3
#define TX1_pin  21
#define RX1_pin  -1
#define TX2_pin  -1
#define RX2_pin  22

#define BUF_SIZE (1024)


void setup() {
  // página 21 del UART.pdf.
  // Las funciones de configuración e instalación (uart_param_config, uart_set_pin, uart_driver_install)
  // y las de comunicación (uart_read_bytes, uart_write_bytes) se detallan en las páginas 14-15 del PDF.
  
  M5.begin(1,0,0,1);
  M5.Lcd.setTextSize(3);
  M5.Lcd.printf("\n\n\n\n");
  M5.Lcd.printf("   Test 3 UARTs\r\n");
  Serial.end(); // Desabilita la UART0

// Inincializacion UARTS
//  uart_config_t uart_config;
//  uart_config.baud_rate = 115200;
//  uart_config.data_bits = UART_DATA_8_BITS;
//  uart_config.parity = UART_PARITY_DISABLE;
//  uart_config.stop_bits = UART_STOP_BITS_1;
//  uart_config.flow_ctrl = UART_HW_FLOWCTRL_DISABLE;
//  uart_config.rx_flow_ctrl_thresh = 120;

    uart_config_t uart_config = {
        .baud_rate = 115200,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE
    };
  
  //uart_param_config(uart_port_t uart_num, const uart_config_t *uart_config);
  uart_param_config(UART_NUM_0, &uart_config);
  uart_param_config(UART_NUM_1, &uart_config);
  uart_param_config(UART_NUM_2, &uart_config);
  
  //uart_set_pin(uart_port_t uart_num, int tx_io_num, int rx_io_num, int rts_io_num, int cts_io_num);
  uart_set_pin(UART_NUM_0, TX0_pin, RX0_pin, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
  uart_set_pin(UART_NUM_1, TX1_pin, RX1_pin, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
  uart_set_pin(UART_NUM_2, TX2_pin, RX2_pin, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
  
  // uart_driver_install(uart_port_t uart_num, int rx_buffer_size, int tx_buffer_size, 
  // int queue_size, QueueHandle_t* uart_queue, int intr_alloc_flags);
  uart_driver_install(UART_NUM_0, BUF_SIZE, 0, 0, NULL, 0);
  uart_driver_install(UART_NUM_1, BUF_SIZE, 0, 0, NULL, 0);
  uart_driver_install(UART_NUM_2, BUF_SIZE, 0, 0, NULL, 0);
  
  uart_write_bytes(UART_NUM_0, "***************\r\n", 17);
  uart_write_bytes(UART_NUM_0, "Prueba 3 UARTs \r\n", 17);
  uart_write_bytes(UART_NUM_0, "***************\r\n", 17);
}


  //uint8_t * datos = (uint8_t*) malloc(BUF_SIZE);
  uint8_t datos[BUF_SIZE];
  int len;

  void loop() {
     len = uart_read_bytes(UART_NUM_0, datos, BUF_SIZE, 20 / portTICK_RATE_MS);
     if (len>0){
       M5.Lcd.setCursor(50,150);
       M5.Lcd.printf("   Len = ");
       M5.Lcd.print(len-1);
     }
     uart_write_bytes(UART_NUM_1, (const char*) datos, len);
     len = uart_read_bytes(UART_NUM_2, datos, BUF_SIZE, 20 / portTICK_RATE_MS);
     uart_write_bytes(UART_NUM_0, (const char*) datos, len);
   }
