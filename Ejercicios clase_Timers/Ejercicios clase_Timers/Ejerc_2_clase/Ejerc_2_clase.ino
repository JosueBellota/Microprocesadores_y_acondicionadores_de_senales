/* Completar el codigo para configurar el timer0, timer1, timer2, timer3, timer4
utilizando funciones de Arduino
- Generar interrupciones periodicas cada  1, 2, 0.5 y 1.5s (fclk=80MHz)
- Activar los timers cuando pulsamos el boton A
- Desactivar los timers cuando pulsamos el boton B
Se dibuja un circulo rojo, verde, azul, blanco con interrupcion 0,1,2,3 respectivamente*/


#include <M5Stack.h>
#define x0 40
#define y0 30
#define r 30
#define x_offset_1 75
#define x_offset_2 2*x_offset_1
#define x_offset_3 3*x_offset_1

// Declarar objetos timer0, timer1, timer2 y timer3





volatile int Flag_ISR_Timer0 = 0;
volatile int Flag_ISR_Timer1 = 0;
volatile int Flag_ISR_Timer2 = 0;
volatile int Flag_ISR_Timer3 = 0;

int Flagcirc_0 = 1;
int Flagcirc_1 = 1;
int Flagcirc_2 = 1;
int Flagcirc_3 = 1;

void IRAM_ATTR ISR_Timer0() {
  Flag_ISR_Timer0 = 1;
}
void IRAM_ATTR ISR_Timer1() {
  Flag_ISR_Timer1 = 1;
}
void IRAM_ATTR ISR_Timer2() {
  Flag_ISR_Timer2 = 1;
}
void IRAM_ATTR ISR_Timer3() {
  Flag_ISR_Timer3 = 1;
}

void setup() {
  M5.begin(1,0,0,1);
  M5.Lcd.setTextSize(3);
  M5.Lcd.printf("\n\n\n\n");
  M5.Lcd.printf("   Test 4 Timers\r\n");
  //Serial.begin(115200);

  Serial.println("  ");
  Serial.println("------------------------------------");
  Serial.println("Prueba 4 Timers");
  Serial.println("------------------------------------");

  // Configurar del Timer 0 y configurar la interrupción 
 


  // Configurar del Timer 1 y configurar la interrupción 
 


  // Configurar del Timer 2 y configurar la interrupción 
  


  // Configurar del Timer 3 y configurar la interrupción 
 


  // Escribir el valor de la alarma con autorecarga
  // Timer 0: alarma 1s
  
      
  // Timer 1: alarma 2s
  

  // Timer 2: alarma 0.5s
 

  // Timer 3: alarma 1.5s




}

void loop() {
  // Control de pulsadores
  M5.BtnA.read();
  if (M5.BtnA.wasPressed()) {
    // Habilitar alarma de los 4 timers
 


    

  }
  M5.BtnB.read();
  if (M5.BtnB.wasPressed()) {
    // Deshabilitar alarma de los 4 timers






  }


  if (Flag_ISR_Timer0) {
    Serial.println("Timer 0");
    if (Flagcirc_0 == 0) {
      M5.Lcd.fillCircle(x0, y0, r, RED); // Dibujar circulo
      Flagcirc_0++;
    }
    else {
      M5.Lcd.fillCircle(x0, y0, r, BLACK); // Borrar circulo
      Flagcirc_0--;
    }
    Flag_ISR_Timer0 = 0;
  }

  if (Flag_ISR_Timer1) {
    Serial.println("       Timer 1");
    if (Flagcirc_1 == 0) {
      M5.Lcd.fillCircle(x0 + x_offset_1, y0, r, GREEN); // Dibujar circulo
      Flagcirc_1++;
    }
    else {
      M5.Lcd.fillCircle(x0 + x_offset_1, y0, r, BLACK); // Borrar circulo
      Flagcirc_1--;
    }
    Flag_ISR_Timer1 = 0;
  }

  if (Flag_ISR_Timer2) {
    Serial.println("              Timer 2");
    if (Flagcirc_2 == 0) {
      M5.Lcd.fillCircle(x0 + x_offset_2, y0, r, BLUE); // Dibujar circulo
      Flagcirc_2++;
    }
    else {
      M5.Lcd.fillCircle(x0 + x_offset_2, y0, r, BLACK); // Borrar circulo
      Flagcirc_2--;
    }
    Flag_ISR_Timer2 = 0;
  }

  if (Flag_ISR_Timer3) {
    Serial.println("                        Timer 3");
    if (Flagcirc_2 == 0) {
      M5.Lcd.fillCircle(x0 + x_offset_3, y0, r, WHITE); // Dibujar circulo
      Flagcirc_3++;
    }
    else {
      M5.Lcd.fillCircle(x0 + x_offset_3, y0, r, BLACK); // Borrar circulo
      Flagcirc_3--;
    }
    Flag_ISR_Timer3 = 0;
  }
}
