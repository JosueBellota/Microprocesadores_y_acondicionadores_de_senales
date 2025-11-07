#include <M5Stack.h>
#include "driver\timer.h"
#define x0 30
#define y0 30
#define r 20
#define TIMER_DIVIDER 80
#define TIMER_SCALE (TIMER_BASE_CLK / TIMER_DIVIDER)  // 80e6/TIMER_DIVIDER
#define TIMER0_INTERVAL0_SEC (1)                      // segundos
#define ESP_INTR_FLAG_TIMER0 ESP_INTR_FLAG_LEVEL3


volatile int Flag_ISR_Timer0 = 0;
volatile uint32_t estado = 0;

// Identificar el grupo y el timer
timer_group_t timer_group = TIMER_GROUP_0;
timer_idx_t timer_idx = TIMER_0;

void IRAM_ATTR ISR_Timer0(void* arg) {  // El arg lleva el número de timer dentro del grupo
  int timer_int_idx = (int)arg;
  uint32_t intr_status = TIMERG0.int_st_timers.val;  // Devuelve 1,2 o 3 si la interrupción
                                                     // viene del T0 (1), el T1(2) o de ambos (3)
                                                     //uint32_t int_status = timer_group_get_intr_status_in_isr(timer_group);

  //if((intr_status & BIT(timer_int_idx)) && timer_int_idx == TIMER_0) {
  // Activar el flag para poner en marcha la rutina asociada
  Flag_ISR_Timer0 = 1;

  // Desactivar el flag de la interrrupción
  TIMERG0.int_clr_timers.t0 = 1;
  //Habilitar la alarma
  timer_set_alarm(TIMER_GROUP_0, TIMER_0, TIMER_ALARM_EN);
  //}
}


void setup() {
  M5.begin(1, 0, 0, 1);
  M5.Lcd.setTextSize(3);
  M5.Lcd.printf("\n\n\n\n");
  M5.Lcd.printf("    Test Timers\r\n");
  Serial.begin(115200);

  Serial.println("  ");
  Serial.println("------------------------------------");
  Serial.println("Prueba Timers");
  Serial.println("------------------------------------");

  // Configuracion Timer0 ////////////////////

  // Definir la configuracion
  timer_config_t conf_timer; 
  conf_timer.alarm_en = TIMER_ALARM_EN;
  conf_timer.auto_reload = TIMER_AUTORELOAD_EN;
  conf_timer.counter_dir = TIMER_COUNT_UP;
  conf_timer.divider = (uint16_t)TIMER_DIVIDER;
  conf_timer.intr_type = TIMER_INTR_LEVEL;
  conf_timer.counter_en = TIMER_PAUSE;

/*
timer_config_t conf_timer = { 
  .alarm_en = TIMER_ALARM_EN;
  .auto_reload = TIMER_AUTORELOAD_EN;
  .counter_dir = TIMER_COUNT_UP;
  .divider = (uint16_t)TIMER_DIVIDER;
  .intr_type = TIMER_INTR_LEVEL;
  .counter_en = TIMER_PAUSE};
*/

  // Inicializar el timer con los parametros de la estructura
  timer_init(timer_group, timer_idx, &conf_timer);
  // Detener el timer
  timer_pause(timer_group, timer_idx);
  // Escribir el valor inicial de la cuenta
  timer_set_counter_value(timer_group, timer_idx, 0x00000000ULL);
  // Escribir el valor de la alarma
  timer_set_alarm_value(timer_group, timer_idx, (TIMER0_INTERVAL0_SEC * TIMER_SCALE));
  // Habilitar la interrupcion del timer
  timer_enable_intr(timer_group, timer_idx);
  // Fijar el manejador de la interrupcion
  timer_isr_register(timer_group, timer_idx, ISR_Timer0, (void*)timer_idx, ESP_INTR_FLAG_TIMER0, NULL);
  //timer_isr_callback_add(timer_group, timer_idx, ISR_Timer0, (void*) timer_idx, ESP_INTR_FLAG_TIMER0); // Last version esp-idf
  // Poner en marcha el timer
  timer_start(timer_group, timer_idx);
}

int Flagcirc_0 = 1;
void loop() {

  // Control de pulsadores
  M5.BtnA.read();
  if (M5.BtnA.wasPressed()) {
    // Poner en marcha el timer
    timer_start(timer_group, timer_idx);
  }
  M5.BtnB.read();
  if (M5.BtnB.wasPressed()) {
    // Detener el timer
    timer_pause(timer_group, timer_idx);
  }

  // Acciones de la ISR 1
  if (Flag_ISR_Timer0 == 1) {
    Serial.println(String("ISR_Timer0 ") + String(((float)millis() / 1000)) + " seg");
    Flag_ISR_Timer0 = 0;
    if (Flagcirc_0 == 0) {
      M5.Lcd.fillCircle(x0, y0, r, RED);  // Dibujar circulo
      Flagcirc_0++;
    } else {
      M5.Lcd.fillCircle(x0, y0, r, BLACK);  // Borrar circulo
      Flagcirc_0--;
    }
  }
}
