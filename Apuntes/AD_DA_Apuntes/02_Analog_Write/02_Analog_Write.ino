/*
  Analog Write (PWM)
  Fades an LED on pin 2 using PWM.
  This example shows how to use the LEDC peripheral to generate a PWM signal.
*/

#include <M5Stack.h>

#define LED_PIN 2

// LEDC channels (0-15), resolution (1-16 bits), and frequency
const int ledcChannel = 0;
const int resolution = 8; // 8-bit resolution (0-255)
const int freq = 5000;    // 5kHz frequency

void setup() {
  //Initialize M5Stack
  M5.begin();
  
  // Configure LEDC channel
  ledcSetup(ledcChannel, freq, resolution);
  
  // Attach pin to LEDC channel
  ledcAttachPin(LED_PIN, ledcChannel);
}

void loop() {
  // increase the LED brightness
  for (int dutyCycle = 0; dutyCycle <= 255; dutyCycle++) {
    ledcWrite(ledcChannel, dutyCycle);
    delay(10);
  }

  // decrease the LED brightness
  for (int dutyCycle = 255; dutyCycle >= 0; dutyCycle--) {
    ledcWrite(ledcChannel, dutyCycle);
    delay(10);
  }
}
