
// 1. Comunicación Serie Básica (para M5Stack)
#include <M5Stack.h>

void setup() {
  // Primero, inicializa el M5Stack.
  M5.begin();

  // Inicia la comunicación serie a una velocidad de 115200 baudios.
  // Es una velocidad común y rápida para el ESP32.
  Serial.begin(115200);

  // No es necesario el `while(!Serial)` en M5Stack de la misma manera que en otras placas,
  // pero no hace daño tenerlo.
  delay(100); // Una pequeña pausa para asegurar que todo se inicialice.

  // --- Limpia la pantalla del M5Stack y muestra un mensaje ---
  M5.Lcd.fillScreen(BLACK);
  M5.Lcd.setCursor(0, 0);
  M5.Lcd.setTextSize(2);
  M5.Lcd.println("Ejemplo 1: Serial Basico");
  M5.Lcd.setTextSize(1);
  M5.Lcd.println("Revisa el Monitor Serie en tu PC.");

  // --- Envía mensajes al Monitor Serie ---
  Serial.println("Hola, Monitor Serie! (Desde M5Stack)");
  Serial.println("Probando Serial.println()");
  
  Serial.print("Esto se imprimira ");
  Serial.print("en la misma linea.");
  Serial.println(" Y esto al final.");
  Serial.println(); // Línea en blanco

  int anio = 2025;
  Serial.print("Año: ");
  Serial.println(anio);
}

void loop() {
  // El loop se deja en blanco. Puedes añadir aquí código que se repita.
  M5.update(); // Actualiza el estado de los botones del M5
}
