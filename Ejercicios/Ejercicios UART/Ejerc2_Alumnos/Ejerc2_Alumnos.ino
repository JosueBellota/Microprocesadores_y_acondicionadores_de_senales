/* CLASE UART Ejem_2*/

#include <M5Stack.h>

#define TX1_pin  21
#define RX1_pin  -1

#define TX2_pin  -1
#define RX2_pin  22

//HardwareSerial Serial1(1);
//HardwareSerial Serial2(2);

void setup() {
  // la página 20 del UART.pdf.
  // La inicialización de UARTs (Serial.begin) y funciones de comunicación (available, read, print)
  // páginas 12-13 del mismo PDF.
  M5.begin();
  M5.Lcd.setTextSize(3);
  M5.Lcd.printf("\n\n\n\n");
  M5.Lcd.printf("   Test 3 UARTs\r\n");
  
//////////////////////////////////////////
  // Inincializacion de las 3 UARTS
  Serial.begin(115200);
  Serial1.begin(115200, SERIAL_8N1, RX1_pin, TX1_pin);
  Serial2.begin(115200, SERIAL_8N1, RX2_pin, TX2_pin);
//////////////////////////////////////////

  Serial.println("  ");
  Serial.println("------------------------------------");
  Serial.println("Ejercicio 2: UARTs");
  Serial.println("------------------------------------");

}


// Declaración de variables
char dato; 
char dato2;
uint8_t num_bytes;

void loop() {


   num_bytes = Serial.available();
   if (num_bytes>0){
    while (num_bytes > 0)
      {
      num_bytes -= 1;
	 ////////////////////////////////////////////////////
	 // Lee datos recibidos por UART0 
      dato = Serial.read();
	  // lo envía a UART1
	  Serial1.print(dato);
	  //////////////////////////////////////////////////////
      }
   }

   num_bytes = Serial2.available();
   if (num_bytes>0){
    while (num_bytes > 0)
      {
      num_bytes -= 1;
	  //////////////////////////////////////////////////
	  // Lee datos recibidos por UART2 
      dato2 = Serial2.read();
      Serial.print("Dato leído = ");
	  // lo envía a UART0
      Serial.print(dato2);
	  /////////////////////////////////////
	  
	  
	  }
   }
  
}
