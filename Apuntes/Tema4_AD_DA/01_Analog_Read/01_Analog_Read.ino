/*
  Analog Read
  Reads an analog input on pin 34, prints the result to the Serial Monitor.
  Attach the center pin of a potentiometer to pin 34, and the outside pins to +3.3V and ground.
*/

#include <M5Stack.h>

void setup() {
  //Initialize M5Stack
  M5.begin();
  
  //Initialize Serial
  Serial.begin(115200);
}

void loop() {
  // read the input on analog pin 34:
  int sensorValue = analogRead(34);
  
  // print out the value you read:
  Serial.println(sensorValue);
  
  delay(100);        // delay in between reads for stability
}
