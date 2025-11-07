/* Completar el codigo para configurar el Timer0
utilizando funciones de Arduino
- Generar una interrupcion periodica cada  1s (fclk=80MHz)
- Activar el timer cuando pulsamos el boton A
- Desactivar el timer cuando pulsamos el boton B
Se dibuja un circulo rojo con cada interrupcion*/

#include <M5Stack.h>
#define x0 30
#define y0 30
#define r 20

// Declaracion objeto Timer0 
hw_timer_t *timer0 = NULL;

volatile int Flag_ISR_Timer0 = 0;

void IRAM_ATTR ISR_Timer0(){
  Flag_ISR_Timer0 = 1;
}

void setup() {
  M5.begin();
  M5.Lcd.setTextSize(3);
  M5.Lcd.printf("\n\n\n\n");
  M5.Lcd.printf("    Test Timers\r\n");
  Serial.begin(115200);
  Serial.println("  ");
  Serial.println("------------------------------------");
  Serial.println("Prueba Timers");
  Serial.println("------------------------------------");

  // Inicializar el Timer0
  // FÓRMULA: timerBegin(timer_num, divider, countUp)
  // - timer_num = 0 (Timer 0 del grupo 0)
  // - divider = 80: 
  //     f_timer = f_clk / divider = 80 MHz / 80 = 1 MHz
  //     Esto significa 1,000,000 ticks por segundo (1 tick cada 1 microsegundo)
  // - countUp = true (contador ascendente)
  timer0 = timerBegin(0, 80, true);

  // Asociar la interrupcion
  timerAttachInterrupt(timer0, &ISR_Timer0, true);
  
  // Escribir el valor de la alarma con autorecarga
  // FÓRMULA: timerAlarmWrite(timer, alarm_value, autoreload)
  // - alarm_value = 1000000:
  //     Tiempo_deseado = alarm_value / f_timer
  //     1 segundo = alarm_value / 1,000,000 Hz
  //     alarm_value = 1 segundo × 1,000,000 Hz = 1,000,000 ticks
  // - autoreload = true (recarga automática para interrupciones periódicas)
  timerAlarmWrite(timer0, 1000000, true);
  
  // Habilitar la alarma del Timer0
  timerAlarmEnable(timer0);
    
  // Inicialmente el timer está pausado
  timerStop(timer0);
}

int Flagcirc_0 = 1;
void loop() {
  
  // Control de pulsadores
  M5.BtnA.read();
  if(M5.BtnA.wasPressed()) {
    // Activar el timer
    timerStart(timer0);
  }
  
  M5.BtnB.read();
  if(M5.BtnB.wasPressed()) {
    // Detener el timer
    timerStop(timer0);
  }
  
  // Acciones de la ISR 1
  if (Flag_ISR_Timer0 == 1){
    Serial.println(String("ISR_Timer0 ")+String(((float) millis()/1000))+" seg");
    Flag_ISR_Timer0 = 0;
    if (Flagcirc_0 == 0){                  
      M5.Lcd.fillCircle(x0, y0, r, RED); // Dibujar circulo
      Flagcirc_0++;
    }
    else{                                
      M5.Lcd.fillCircle(x0, y0, r, BLACK); // Borrar circulo
      Flagcirc_0--;
    }  
  }
  
  M5.update(); // Actualizar estado de los botones
}