int pwmValue = 255;  
bool decreasing = true;

void setup() {
  Serial.begin(115200);
  pinMode(18, OUTPUT);
  pinMode(19, OUTPUT);
  digitalWrite(19, LOW);  
}

void loop() {
  
  analogWrite(18, pwmValue);
  
  // Print current PWM value
  Serial.print("PWM Value: ");
  Serial.println(pwmValue);
  
  
  if (decreasing) {
    pwmValue -= 5;
    if (pwmValue <= 0) {
      pwmValue = 0;
      decreasing = false;
    }
  } else {
    pwmValue += 5;
    if (pwmValue >= 255) {
      pwmValue = 255;
      decreasing = true;
    }
  }
  
  delay(50); 
}