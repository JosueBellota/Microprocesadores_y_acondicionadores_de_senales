/*
  External Interrupt
  Triggers an interrupt when a button is pressed on a specific pin.
  Connect a button to pin 36 (GPIO36) and ground.
*/

#include <M5Stack.h>

#define BUTTON_PIN 36 // M5Stack A button is connected to GPIO36

// Global variable to store the interrupt count
volatile int interruptCounter = 0;

// Interrupt Service Routine (ISR)
void IRAM_ATTR handleInterrupt() {
  interruptCounter++;
}

void setup() {
  //Initialize M5Stack
  M5.begin();
  
  // Set up Serial Monitor
  Serial.begin(115200);
  Serial.println("External Interrupt Example");
  Serial.print("Connect a button to GPIO ");
  Serial.print(BUTTON_PIN);
  Serial.println(" and GND.");

  // Configure the button pin as input with a pull-up resistor
  pinMode(BUTTON_PIN, INPUT_PULLUP);

  // Attach the interrupt to the button pin
  // FALLING means the interrupt will be triggered when the button is pressed (goes from HIGH to LOW)
  attachInterrupt(digitalPinToInterrupt(BUTTON_PIN), handleInterrupt, FALLING);
}

void loop() {
  // Print the interrupt counter every second
  static unsigned long lastPrintTime = 0;
  if (millis() - lastPrintTime > 1000) {
    Serial.print("Interrupt Count: ");
    Serial.println(interruptCounter);
    lastPrintTime = millis();
  }
}
