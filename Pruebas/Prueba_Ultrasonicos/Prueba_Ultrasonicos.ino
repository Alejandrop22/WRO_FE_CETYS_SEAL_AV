
#define TRIG1 26
#define ECHO1 25
#define TRIG2 32
#define ECHO2 33



void setup() {
  Serial.begin(115200);
  

  pinMode(TRIG1, OUTPUT);
  pinMode(ECHO1, INPUT);
  pinMode(TRIG2, OUTPUT);
  pinMode(ECHO2, INPUT);
  

  digitalWrite(TRIG1, LOW);
  digitalWrite(TRIG2, LOW);
  
  delay(1000);
  Serial.println("Sensores ultrasonidos listos (pulseIn)");
  Serial.println("Sensor1 (26/25) | Sensor2 (32/33)");
}

void loop() {

  float distancia1 = leerDistancia(TRIG1, ECHO1);
  

  float distancia2 = leerDistancia(TRIG2, ECHO2);
  
  // Mostrar resultados
  Serial.print("Sensor1: ");
  Serial.print(distancia1);
  Serial.print(" cm\t\tSensor2: ");
  Serial.print(distancia2);
  Serial.println(" cm");
  
  delay(200); 
}


float leerDistancia(int trigPin, int echoPin) {

  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  

  long duracion = pulseIn(echoPin, HIGH, 30000);
  

  if (duracion == 0) {
    return 999.0; // Valor indicando "sin eco"
  }
  

  float distancia = duracion * 0.034 / 2;
  

  if (distancia > 400) distancia = 400;
  
  return distancia;
}