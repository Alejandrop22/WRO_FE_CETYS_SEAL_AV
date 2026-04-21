#include <ESP32Servo.h>

#define SERVO_PIN 13

Servo miServo;
double pos = 45;           
double incremento = 1;     

void setup() {
  miServo.attach(SERVO_PIN);
  miServo.write(pos);
  delay(500);
}

void loop() {

  miServo.write(pos);
  delay(15);            
  
  pos = pos + incremento;
  
  if (pos >= 145) {
    pos = 145;
    incremento = -1;   
  } else if (pos <= 45) {
    pos = 45;
    incremento = 1;   
  }

  delay(100);
}