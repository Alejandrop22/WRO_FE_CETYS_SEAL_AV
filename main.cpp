#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_VL53L1X.h>
#include <Adafruit_MPU6050.h>
#include <MadgwickAHRS.h>

// ================== PINES SENSORES ==================
#define XSHUT_RIGHT 15
#define XSHUT_LEFT  13

#define LedPin 18
#define BotonPin 23
#define EncoderPin 19


// ========== IMU ==========
Adafruit_MPU6050 mpu;
Madgwick fusion;
bool imuDisponible = false;
float ax_offset=0, ay_offset=0, az_offset=0;
float gx_offset=0, gy_offset=0, gz_offset=0;
float roll, pitch, yaw;
const int CALIBRACIONES = 1200;
const int CALIBRACIONES_WARMUP = 200;
int imuYawAxis = 2;
double yawIntegrado = 0.0;
unsigned long ultimoTiempoYaw = 0;

const float GYRO_DEADBAND_RADS = 0.01f;
const float GYRO_QUIETO_RADS = 0.02f;

// ============================================================
// MODELO DE DERIVA LINEAL DEL BIAS DEL GIROSCOPIO
// ============================================================
float gyroBiasPrev[3]  = {0.0f, 0.0f, 0.0f};
float gyroDriftRate[3] = {0.0f, 0.0f, 0.0f};
unsigned long biasTimestamp = 0;
const float MAX_DRIFT_RATE_RADS_S = 0.01f;
const float MAX_EXTRAPOLACION_SEG = 15.0f;

float offsetDinamico(int eje) {
  float dtSeg = (millis() - biasTimestamp) / 1000.0f;
  if (dtSeg > MAX_EXTRAPOLACION_SEG) dtSeg = MAX_EXTRAPOLACION_SEG;
  if (dtSeg < 0) dtSeg = 0;
  return gyroBiasPrev[eje] + gyroDriftRate[eje] * dtSeg;
}

// ============================================================
// Calibración inicial completa
// ============================================================
bool calibrarIMU() {
  if (!imuDisponible) {
    Serial.println("IMU no disponible, se omite calibracion");
    return false;
  }

  const int MAX_INTENTOS = 3;
  for (int intento = 1; intento <= MAX_INTENTOS; intento++) {
    Serial.print("Calibrando IMU (intento "); Serial.print(intento);
    Serial.println(")... NO MUEVAS EL ROBOT");

    double as=0.0, bs=0.0, cs=0.0, ds=0.0, es=0.0, fs=0.0;
    float gxMin=1e6, gxMax=-1e6, gyMin=1e6, gyMax=-1e6, gzMin=1e6, gzMax=-1e6;
    int muestrasValidas = 0;

    for (int i = 0; i < CALIBRACIONES; i++) {
      sensors_event_t a, g, temp;
      mpu.getEvent(&a, &g, &temp);

      if (i >= CALIBRACIONES_WARMUP) {
        as += a.acceleration.x; bs += a.acceleration.y; cs += a.acceleration.z;
        ds += g.gyro.x; es += g.gyro.y; fs += g.gyro.z;
        muestrasValidas++;

        if (g.gyro.x < gxMin) gxMin = g.gyro.x; if (g.gyro.x > gxMax) gxMax = g.gyro.x;
        if (g.gyro.y < gyMin) gyMin = g.gyro.y; if (g.gyro.y > gyMax) gyMax = g.gyro.y;
        if (g.gyro.z < gzMin) gzMin = g.gyro.z; if (g.gyro.z > gzMax) gzMax = g.gyro.z;
      }

      delay(3);
      if ((i & 255) == 0) Serial.println(i);
      if (i % 5 == 0) digitalWrite(LedPin, !digitalRead(LedPin));
    }

    float rangoMax = max(gxMax - gxMin, max(gyMax - gyMin, gzMax - gzMin));
    if (rangoMax > GYRO_QUIETO_RADS * 4.0f) {
      Serial.print("Calibracion descartada: se detecto movimiento (rango=");
      Serial.print(rangoMax); Serial.println(" rad/s). Reintentando...");
      continue;
    }

    float ax_medio = as / muestrasValidas / 9.81f;
    float ay_medio = bs / muestrasValidas / 9.81f;
    float az_medio = cs / muestrasValidas / 9.81f;
    imuYawAxis = 0;
    if (fabsf(ay_medio) > fabsf(ax_medio)) imuYawAxis = 1;
    if (fabsf(az_medio) > fabsf(imuYawAxis == 0 ? ax_medio : ay_medio)) imuYawAxis = 2;

    ax_offset = 0;
    ay_offset = 0;
    az_offset = 0;
    gx_offset = ds / muestrasValidas;
    gy_offset = es / muestrasValidas;
    gz_offset = fs / muestrasValidas;

    Serial.print("Offsets Acel: ");
    Serial.print(ax_offset); Serial.print(", ");
    Serial.print(ay_offset); Serial.print(", ");
    Serial.println(az_offset);
    Serial.print("Offsets Giro: ");
    Serial.print(gx_offset, 6); Serial.print(", ");
    Serial.print(gy_offset, 6); Serial.print(", ");
    Serial.println(gz_offset, 6);
    Serial.print("Eje Yaw detectado: ");
    Serial.println(imuYawAxis == 0 ? "X" : (imuYawAxis == 1 ? "Y" : "Z"));
    Serial.println("Calibracion OK");

    gyroBiasPrev[0] = gx_offset;
    gyroBiasPrev[1] = gy_offset;
    gyroBiasPrev[2] = gz_offset;
    gyroDriftRate[0] = gyroDriftRate[1] = gyroDriftRate[2] = 0.0f;
    biasTimestamp = millis();

    return true;
  }

  Serial.println("ADVERTENCIA: no se logro una calibracion limpia tras varios intentos. Se usa la ultima medida igual.");
  return false;
}

// ============================================================
// Recalibración rápida del bias del giroscopio
// ============================================================
void recalibrarGyroRapido() {
  if (!imuDisponible) return;

  const int MUESTRAS = 60;
  double ds = 0.0, es = 0.0, fs = 0.0;
  float gxMin=1e6, gxMax=-1e6, gyMin=1e6, gyMax=-1e6, gzMin=1e6, gzMax=-1e6;

  for (int i = 0; i < MUESTRAS; i++) {
    sensors_event_t a, g, temp;
    mpu.getEvent(&a, &g, &temp);

    ds += g.gyro.x; es += g.gyro.y; fs += g.gyro.z;
    if (g.gyro.x < gxMin) gxMin = g.gyro.x; if (g.gyro.x > gxMax) gxMax = g.gyro.x;
    if (g.gyro.y < gyMin) gyMin = g.gyro.y; if (g.gyro.y > gyMax) gyMax = g.gyro.y;
    if (g.gyro.z < gzMin) gzMin = g.gyro.z; if (g.gyro.z > gzMax) gzMax = g.gyro.z;

    delay(2);
  }

  float rangoMax = max(gxMax - gxMin, max(gyMax - gyMin, gzMax - gzMin));
  if (rangoMax > GYRO_QUIETO_RADS * 4.0f) {
    Serial.println("[RECAL] Robot no esta quieto, se mantiene el offset anterior");
    return;
  }

  float nuevoGx = ds / MUESTRAS;
  float nuevoGy = es / MUESTRAS;
  float nuevoGz = fs / MUESTRAS;

  unsigned long ahora = millis();
  float dtSeg = (ahora - biasTimestamp) / 1000.0f;

  if (dtSeg > 0.3f) {
    float nuevaTasaX = (nuevoGx - gyroBiasPrev[0]) / dtSeg;
    float nuevaTasaY = (nuevoGy - gyroBiasPrev[1]) / dtSeg;
    float nuevaTasaZ = (nuevoGz - gyroBiasPrev[2]) / dtSeg;

    nuevaTasaX = constrain(nuevaTasaX, -MAX_DRIFT_RATE_RADS_S, MAX_DRIFT_RATE_RADS_S);
    nuevaTasaY = constrain(nuevaTasaY, -MAX_DRIFT_RATE_RADS_S, MAX_DRIFT_RATE_RADS_S);
    nuevaTasaZ = constrain(nuevaTasaZ, -MAX_DRIFT_RATE_RADS_S, MAX_DRIFT_RATE_RADS_S);

    const float ALPHA_TASA = 0.5f;
    gyroDriftRate[0] = gyroDriftRate[0] * (1.0f - ALPHA_TASA) + nuevaTasaX * ALPHA_TASA;
    gyroDriftRate[1] = gyroDriftRate[1] * (1.0f - ALPHA_TASA) + nuevaTasaY * ALPHA_TASA;
    gyroDriftRate[2] = gyroDriftRate[2] * (1.0f - ALPHA_TASA) + nuevaTasaZ * ALPHA_TASA;

    Serial.print("[RECAL] Tasa de deriva Z: "); Serial.print(gyroDriftRate[2], 6);
    Serial.println(" rad/s por segundo");
  }

  gyroBiasPrev[0] = nuevoGx;
  gyroBiasPrev[1] = nuevoGy;
  gyroBiasPrev[2] = nuevoGz;
  biasTimestamp = ahora;

  gx_offset = nuevoGx;
  gy_offset = nuevoGy;
  gz_offset = nuevoGz;

  Serial.print("[RECAL] Nuevo offset giro Z: "); Serial.println(gz_offset, 6);
}

// ============================================================
void leerIMU() {
  if (!imuDisponible) return;

  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);

  float axf = (a.acceleration.x / 9.81f) - ax_offset;
  float ayf = (a.acceleration.y / 9.81f) - ay_offset;
  float azf = (a.acceleration.z / 9.81f) - az_offset;
  float gxf = g.gyro.x - offsetDinamico(0);
  float gyf = g.gyro.y - offsetDinamico(1);
  float gzf = g.gyro.z - offsetDinamico(2);

  fusion.updateIMU(gxf, gyf, gzf, axf, ayf, azf);

  roll  = fusion.getRoll();
  pitch = fusion.getPitch();
  unsigned long ahoraYaw = micros();
  if (ultimoTiempoYaw != 0) {
    double deltaSegundos = (ahoraYaw - ultimoTiempoYaw) / 1000000.0;
    float velocidadYaw = imuYawAxis == 0 ? gxf : (imuYawAxis == 1 ? gyf : gzf);

    if (fabsf(velocidadYaw) > GYRO_DEADBAND_RADS) {
      yawIntegrado += (double)velocidadYaw * deltaSegundos * 57.29577951;
    }

    while (yawIntegrado > 180.0) yawIntegrado -= 360.0;
    while (yawIntegrado < -180.0) yawIntegrado += 360.0;
  }
  ultimoTiempoYaw = ahoraYaw;
  yaw = (float)yawIntegrado;
}


volatile long encoderCount = 0;

void IRAM_ATTR onEncoder() {
  encoderCount++;
}

void resetEncoder() {
  noInterrupts();
  encoderCount = 0;
  interrupts();
}

float getEncoderCentimeters() {
  noInterrupts();
  long val = encoderCount;
  interrupts();
  return (val / 44.0f) * 2.51f;
}

// ================== DIRECCIONES I2C ==================
#define ADDR_S1     0x30
#define ADDR_S2     0x31

#define ADELANTE 1
#define ATRAS    2

enum Estado {
  BUSCAR_PARED_INICIAL,
  SEGUIR_PARED_DERECHA,
  SEGUIR_PARED_IZQUIERDA,
  GIRO_DERECHA,
  GIRO_IZQUIERDA,
  PARAR_BLINKEAR
};

Estado estado = BUSCAR_PARED_INICIAL;

float imuActual = 0.0f;
float yawObjetivo = 0.0f;

int giroCount = 0;
const int MAX_GIROS = 12;

// ===== Distancia de avance post‑giro =====
float forwardAfterCenterCmForCurrentTurn(int currentTurnIndex) {
  if (currentTurnIndex == 0) {
    return 30.0f;
  } else {
    return 5.0f;
  }
}

// ===== Distancia de giro: 29 cm solo en el primer giro, 25 cm después =====
float turnDistanceCmForCurrentTurn(int currentTurnIndex) {
  if (currentTurnIndex == 0) {
    return 29.0f;   // Primer giro
  } else {
    return 25.0f;   // Giros siguientes
  }
}

Adafruit_VL53L1X sensor_right = Adafruit_VL53L1X();
Adafruit_VL53L1X sensor_left  = Adafruit_VL53L1X();

const unsigned long INTERVALO_LECTURA = 50;
unsigned long ultimaLectura = 0;

const uint16_t UMBRAL_ESQUINA = 200;

bool robotDetenido = false;

bool s_right_ok = false, s_left_ok = false;
bool sensorRightActivo = true;
bool sensorLeftActivo  = true;

int MotorPin1 = 2;
int MotorPin2 = 4;
int MotorFrecuency = 5000;
int MotorResolution = 8;

int ServoPin = 25;
int ServoChannel = 2;
int ServoFrequency = 50;
int ServoResolution = 16;
int ServoMin = 60;
int ServoMax = 120;
int SERVO_DUTY_MIN = 1638;
int SERVO_DUTY_MAX = 8191;
int servoCenter = (ServoMin + ServoMax) / 2;
int anguloActual = servoCenter;

const int VELOCIDAD_BASE = 255;
const int VELOCIDAD_SEGUIR = VELOCIDAD_BASE - 30;   // 225

float Kp = 0.28f;
float Ki = 0.006f;
float Kd = 0.055f;
float pid_integral = 0.0f;
float pid_prev_error = 0.0f;
unsigned long lastPIDTime = 0;
const float INTEGRAL_MAX = 300.0f;

uint16_t lastDistR = 9999;
uint16_t lastDistL = 9999;
unsigned long lastUpdateR = 0;
unsigned long lastUpdateL = 0;

const unsigned long DATA_STALE_MS = 150;

unsigned long buscarStartTime = 0;
bool buscarTimerRunning = false;
const unsigned long BUSCAR_TURN_DELAY_MS = 1000;

// ===== Variables para PARAR_BLINKEAR =====
bool pararBuscandoTerminado = false;
bool ultimoLadoDerecha = true;  // se actualiza al entrar en SEGUIR_PARED_*

#include "soc/soc.h"
#include "soc/rtc_cntl_reg.h"

int servoAngleToDuty(int angle) {
  return map(angle, 0, 180, SERVO_DUTY_MIN, SERVO_DUTY_MAX);
}

void moverServo(int angle) {
  ledcWrite(ServoChannel, servoAngleToDuty(angle));
}

void motorVelocity(int velocity, int direction) {
  if (velocity <= 0) {
    ledcWrite(0, 0);
    ledcWrite(1, 0);
    return;
  }

  if (direction == ADELANTE) {
    ledcWrite(0, velocity);
    ledcWrite(1, 0);
  } else {
    ledcWrite(0, 0);
    ledcWrite(1, velocity);
  }
}

void motorStop() {
  ledcWrite(0, 255);
  ledcWrite(1, 255);
}

void activarSensoresParaLado(bool siguiendoDerecha) {
  if (siguiendoDerecha) {
    if (s_left_ok && sensorLeftActivo) {
      sensor_left.stopRanging();
      sensorLeftActivo = false;
      Serial.println("[SENSORES] Izquierdo apagado (siguiendo pared derecha)");
    }
    if (s_right_ok && !sensorRightActivo) {
      sensor_right.startRanging();
      sensorRightActivo = true;
      Serial.println("[SENSORES] Derecho encendido");
    }
  } else {
    if (s_right_ok && sensorRightActivo) {
      sensor_right.stopRanging();
      sensorRightActivo = false;
      Serial.println("[SENSORES] Derecho apagado (siguiendo pared izquierda)");
    }
    if (s_left_ok && !sensorLeftActivo) {
      sensor_left.startRanging();
      sensorLeftActivo = true;
      Serial.println("[SENSORES] Izquierdo encendido");
    }
  }
}

void activarAmbosSensores() {
  if (s_right_ok && !sensorRightActivo) {
    sensor_right.startRanging();
    sensorRightActivo = true;
  }
  if (s_left_ok && !sensorLeftActivo) {
    sensor_left.startRanging();
    sensorLeftActivo = true;
  }
}

void entrarGiroMismoLado(bool siguiendoDerecha) {
  estado = siguiendoDerecha ? GIRO_DERECHA : GIRO_IZQUIERDA;
}

uint16_t leerSensor(Adafruit_VL53L1X &sensor, uint16_t &lastValue, unsigned long &lastUpdate, bool &fresh) {
  if (sensor.dataReady()) {
    int16_t d = sensor.distance();
    sensor.clearInterrupt();
    if (d != -1) {
      lastValue = (uint16_t)d;
      lastUpdate = millis();
    }
  }

  fresh = (millis() - lastUpdate) <= DATA_STALE_MS;
  return lastValue;
}

void setup() {
  WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0);
  Serial.begin(115200);
  delay(100);
  Serial.println("[SETUP] Inicio");

  pinMode(LedPin, OUTPUT);

  Wire.begin(17, 16);
  Serial.println("[SETUP] Wire iniciado");

  imuDisponible = mpu.begin();
  if (imuDisponible) {
    Serial.println("[SETUP] MPU6050 conectado");
    mpu.setAccelerometerRange(MPU6050_RANGE_2_G);
    mpu.setGyroRange(MPU6050_RANGE_250_DEG);
    mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);
    fusion.begin(20.0f);
  } else {
    Serial.println("[SETUP] WARNING: MPU6050 NO responde");
  }

  calibrarIMU();

  ledcSetup(0, MotorFrecuency, MotorResolution);
  ledcAttachPin(MotorPin1, 0);
  ledcSetup(1, MotorFrecuency, MotorResolution);
  ledcAttachPin(MotorPin2, 1);
  ledcSetup(ServoChannel, ServoFrequency, ServoResolution);
  ledcAttachPin(ServoPin, ServoChannel);

  pinMode(XSHUT_RIGHT, OUTPUT);
  pinMode(XSHUT_LEFT, OUTPUT);

  pinMode(EncoderPin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(EncoderPin), onEncoder, RISING);
  Serial.println("[SETUP] Encoder attached on pin 19");

  digitalWrite(XSHUT_RIGHT, LOW);
  digitalWrite(XSHUT_LEFT, LOW);
  delay(20);
  Serial.println("[SETUP] XSHUT ambos LOW");

  // Sensor derecho
  digitalWrite(XSHUT_RIGHT, HIGH);
  delay(50);
  Serial.println("[SETUP] XSHUT derecho HIGH, intentando begin()...");
  if (sensor_right.begin(0x29, &Wire)) {
    Serial.println("[SETUP] sensor_right.begin(0x29) devolvio true");
    sensor_right.VL53L1X_SetI2CAddress(ADDR_S1);
    s_right_ok = sensor_right.begin(ADDR_S1, &Wire);
    Serial.print("[SETUP] sensor_right.begin(readdress) result: "); Serial.println(s_right_ok);
    if (s_right_ok) {
      sensor_right.VL53L1X_SetDistanceMode(3);
      sensor_right.setTimingBudget(140);
      sensor_right.VL53L1X_SetROI(8, 8);
      sensor_right.startRanging();
      sensorRightActivo = true;
      Serial.println("Sensor Derecho OK");
    } else {
      Serial.println("ERROR al reconectar - Sensor Derecho");
    }
  } else {
    Serial.println("ERROR Sensor Derecho (begin)");
  }

  // Sensor izquierdo
  digitalWrite(XSHUT_LEFT, HIGH);
  delay(50);
  Serial.println("[SETUP] XSHUT izquierdo HIGH, intentando begin()...");
  if (sensor_left.begin(0x29, &Wire)) {
    Serial.println("[SETUP] sensor_left.begin(0x29) devolvio true");
    sensor_left.VL53L1X_SetI2CAddress(ADDR_S2);
    s_left_ok = sensor_left.begin(ADDR_S2, &Wire);
    Serial.print("[SETUP] sensor_left.begin(readdress) result: "); Serial.println(s_left_ok);
    if (s_left_ok) {
      sensor_left.VL53L1X_SetDistanceMode(3);
      sensor_left.setTimingBudget(140);
      sensor_left.VL53L1X_SetROI(8, 8);
      sensor_left.startRanging();
      sensorLeftActivo = true;
      Serial.println("Sensor Izquierdo OK");
    } else {
      Serial.println("ERROR al reconectar - Sensor Izquierdo");
    }
  } else {
    Serial.println("ERROR Sensor Izquierdo (begin)");
  }

  moverServo(servoCenter);
  Serial.println("[SETUP] setup completo");

  pinMode(BotonPin, INPUT_PULLUP);
  Serial.println("Presiona el boton para iniciar...");
  Serial.println("[BOTON] GPIO23 configurado como INPUT_PULLUP; conecta el boton entre GPIO23 y GND");

  unsigned long ultimoReporteBoton = 0;
  while (true) {
    int nivelBoton = digitalRead(BotonPin);
    if (nivelBoton == LOW) {
      delay(50);
      if (digitalRead(BotonPin) == LOW) {
        break;
      }
    }

    if (millis() - ultimoReporteBoton >= 1000) {
      ultimoReporteBoton = millis();
      Serial.print("[BOTON] GPIO23 = ");
      Serial.println(nivelBoton == HIGH ? "HIGH (sin pulsar)" : "LOW (pulsado)");
    }
    delay(10);
  }
  Serial.println("Boton pulsado, iniciando...");

  lastPIDTime = millis();

  estado = BUSCAR_PARED_INICIAL;
}

void loop() {

  leerIMU();

  if(estado == BUSCAR_PARED_INICIAL) {
    activarAmbosSensores();

    if (!buscarTimerRunning) {
      buscarStartTime = millis();
      buscarTimerRunning = true;
      Serial.println("[BUSCAR] Temporizador iniciado (1000 ms)");
    }

    unsigned long tiempoDesdeInicio = millis() - buscarStartTime;
    int velocidadArranque = tiempoDesdeInicio < 500
      ? map(tiempoDesdeInicio, 0, 500, 100, VELOCIDAD_BASE)
      : VELOCIDAD_BASE;
    motorVelocity(velocidadArranque, ADELANTE);

    bool freshL = true, freshR = true;
    uint16_t distL = s_left_ok  ? leerSensor(sensor_left,  lastDistL, lastUpdateL, freshL) : 9999;
    uint16_t distR = s_right_ok ? leerSensor(sensor_right, lastDistR, lastUpdateR, freshR) : 9999;

    Serial.print("Derecha: "); Serial.print(distR);
    Serial.print(freshR ? " mm (OK)" : " mm (CONGELADO/SIN DATOS)");
    Serial.print(" | Izquierda: "); Serial.print(distL);
    Serial.println(freshL ? " mm (OK)" : " mm (CONGELADO/SIN DATOS)");

    bool sensorFrozenLeft = (s_left_ok && !freshL);
    bool sensorFrozenRight = (s_right_ok && !freshR);
    bool sensorCongelado = sensorFrozenLeft || sensorFrozenRight;

    if (sensorCongelado) {
      Serial.print("SENSOR CONGELADO - deteniendo por seguridad. Sensores congelados:");
      if (sensorFrozenLeft) Serial.print(" IZQUIERDO");
      if (sensorFrozenRight) Serial.print(" DERECHO");
      Serial.println();
      digitalWrite(LedPin, HIGH);
      motorStop();
    } else if (distL > 1000 || distR > 1000 || sensorCongelado) {
      Serial.println("No hay pared cercana, deteniendo robot.");
      digitalWrite(LedPin, HIGH);
      motorStop();
      delay(500);
    } else {
      motorVelocity(velocidadArranque, ADELANTE);
      digitalWrite(LedPin, LOW);
    }

    if (sensorFrozenLeft || sensorFrozenRight) {
      if (millis() - buscarStartTime >= BUSCAR_TURN_DELAY_MS) {
        if (sensorFrozenLeft) {
          estado = GIRO_IZQUIERDA;
          buscarTimerRunning = false;
        } else if (sensorFrozenRight) {
          estado = GIRO_DERECHA;
          buscarTimerRunning = false;
        }
      } else {
        unsigned long remaining = BUSCAR_TURN_DELAY_MS - (millis() - buscarStartTime);
        Serial.print("[BUSCAR] Giro bloqueado por "); Serial.print(remaining); Serial.println(" ms más");
      }
    }
  }

  // ================== BLOQUE DE GIRO MODIFICADO ==================
  if (estado == GIRO_DERECHA || estado == GIRO_IZQUIERDA) {
    const bool giroDerecha = estado == GIRO_DERECHA;
    const char *nombreGiro = giroDerecha ? "[GIRO_DERECHA]" : "[GIRO_IZQUIERDA]";

    digitalWrite(LedPin, HIGH);
    motorStop();
    delay(100);

    recalibrarGyroRapido();

    motorStop();
    delay(200);

    moverServo(giroDerecha ? ServoMax : ServoMin);

    resetEncoder();
    motorVelocity(VELOCIDAD_BASE - 10, ADELANTE);

    Serial.print(nombreGiro);
    // ===== Usar la nueva función para la distancia de giro =====
    float DIST_GIRO_CM = turnDistanceCmForCurrentTurn(giroCount);
    Serial.print(" Girando por encoder hasta ");
    Serial.print(DIST_GIRO_CM);
    Serial.println(" cm...");

    unsigned long turnStart = millis();
    while (getEncoderCentimeters() < DIST_GIRO_CM) {
      if (millis() - turnStart > 5000) {
        Serial.println("[GIRO] Timeout de seguridad, cortando giro");
        break;
      }
      delay(1);
    }

    motorStop();
    delay(100);

    moverServo(servoCenter);
    delay(100);
    motorVelocity(VELOCIDAD_SEGUIR + 10, ADELANTE);
    resetEncoder();

    float distanciaAvance = forwardAfterCenterCmForCurrentTurn(giroCount);
    Serial.print(nombreGiro);
    Serial.print(" Avanzando recto ");
    Serial.print(distanciaAvance);
    Serial.println(" cm");

    while (getEncoderCentimeters() < distanciaAvance) {
      delay(1);
    }

    motorStop();
    delay(1000);

    recalibrarGyroRapido();

    giroCount++;
    if (giroCount >= MAX_GIROS) {
      Serial.println("[GIRO] Max giros alcanzado, entrando en modo PARAR_BLINKEAR");
      pararBuscandoTerminado = false;
      resetEncoder();
      estado = PARAR_BLINKEAR;
    } else {
      estado = giroDerecha ? SEGUIR_PARED_DERECHA : SEGUIR_PARED_IZQUIERDA;
      imuActual = yawObjetivo;
      ultimoLadoDerecha = giroDerecha;
      activarSensoresParaLado(giroDerecha);
    }
  }

  // ================== SEGUIMIENTO DE PARED (sin IMU, con PID) ==================
  if (estado == SEGUIR_PARED_DERECHA || estado == SEGUIR_PARED_IZQUIERDA) {
    const bool siguiendoDerecha = estado == SEGUIR_PARED_DERECHA;
    const char *nombreEstado = siguiendoDerecha
      ? "[SEGUIR_PARED_DERECHA]"
      : "[SEGUIR_PARED_IZQUIERDA]";

    digitalWrite(LedPin, LOW);
    motorVelocity(VELOCIDAD_SEGUIR, ADELANTE);

    bool fresh = true;
    uint16_t distancia = siguiendoDerecha
      ? (s_right_ok ? leerSensor(sensor_right, lastDistR, lastUpdateR, fresh) : 9999)
      : (s_left_ok ? leerSensor(sensor_left, lastDistL, lastUpdateL, fresh) : 9999);

    if (!fresh || distancia > 400) {
      Serial.print(nombreEstado);
      if (!fresh) {
        Serial.print(" Sensor ");
        Serial.print(siguiendoDerecha ? "derecho" : "izquierdo");
        Serial.println(" no disponible (CONGELADO). Deteniendo por seguridad.");
      } else {
        Serial.print(" Distancia excesiva: ");
        Serial.print(distancia);
        Serial.println(" mm (>400). Lanzando giro.");
      }
      motorStop();
      delay(50);
      entrarGiroMismoLado(siguiendoDerecha);
    } else {
      unsigned long now = millis();
      float dt = (now - lastPIDTime) / 1000.0f;
      if (dt <= 0) dt = 0.001f;
      lastPIDTime = now;

      const int DESIRED_MM = 100;
      float error = (float)distancia - (float)DESIRED_MM;

      pid_integral += error * dt;
      if (pid_integral > INTEGRAL_MAX) pid_integral = INTEGRAL_MAX;
      if (pid_integral < -INTEGRAL_MAX) pid_integral = -INTEGRAL_MAX;

      float derivada = (error - pid_prev_error) / dt;
      pid_prev_error = error;

      int signoLado = siguiendoDerecha ? 1 : -1;
      int deltaAngle = (int)(signoLado * (Kp * error + Ki * pid_integral + Kd * derivada));

      int newAngle = servoCenter + deltaAngle;
      if (newAngle < ServoMin) newAngle = ServoMin;
      if (newAngle > ServoMax) newAngle = ServoMax;

      moverServo(newAngle);
      anguloActual = newAngle;
      motorVelocity(VELOCIDAD_SEGUIR, ADELANTE);

      Serial.print(nombreEstado);
      Serial.print(" dist="); Serial.print(distancia);
      Serial.print(" mm, servo="); Serial.println(newAngle);
      Serial.print("PID: error="); Serial.print(error);
      Serial.print(", integral="); Serial.print(pid_integral);
      Serial.print(", deriv="); Serial.println(derivada);
      delay(50);
    }
  }

  // ================== PARAR BLINKEAR ==================
  if (estado == PARAR_BLINKEAR) {
    if (!pararBuscandoTerminado) {
      const bool siguiendoDerecha = ultimoLadoDerecha;
      const char *nombreEstado = siguiendoDerecha ? "[PARAR_BLINKEAR_DER]" : "[PARAR_BLINKEAR_IZQ]";

      digitalWrite(LedPin, LOW);
      motorVelocity(VELOCIDAD_SEGUIR, ADELANTE);

      bool fresh = true;
      uint16_t distancia = siguiendoDerecha
        ? (s_right_ok ? leerSensor(sensor_right, lastDistR, lastUpdateR, fresh) : 9999)
        : (s_left_ok ? leerSensor(sensor_left, lastDistL, lastUpdateL, fresh) : 9999);

      if (!fresh) {
        Serial.print(nombreEstado);
        Serial.println(" Sensor no disponible, deteniendo sin completar.");
        motorStop();
        pararBuscandoTerminado = true;
      } else {
        unsigned long now = millis();
        float dt = (now - lastPIDTime) / 1000.0f;
        if (dt <= 0) dt = 0.001f;
        lastPIDTime = now;

        const int DESIRED_MM = 100;
        float error = (float)distancia - (float)DESIRED_MM;

        pid_integral += error * dt;
        if (pid_integral > INTEGRAL_MAX) pid_integral = INTEGRAL_MAX;
        if (pid_integral < -INTEGRAL_MAX) pid_integral = -INTEGRAL_MAX;

        float derivada = (error - pid_prev_error) / dt;
        pid_prev_error = error;

        int signoLado = siguiendoDerecha ? 1 : -1;
        int deltaAngle = (int)(signoLado * (Kp * error + Ki * pid_integral + Kd * derivada));

        int newAngle = servoCenter + deltaAngle;
        if (newAngle < ServoMin) newAngle = ServoMin;
        if (newAngle > ServoMax) newAngle = ServoMax;

        moverServo(newAngle);
        anguloActual = newAngle;
        motorVelocity(VELOCIDAD_SEGUIR, ADELANTE);

        Serial.print(nombreEstado);
        Serial.print(" dist="); Serial.print(distancia);
        Serial.print(" mm, servo="); Serial.println(newAngle);
        Serial.print("PID: error="); Serial.print(error);
        Serial.print(", integral="); Serial.print(pid_integral);
        Serial.print(", deriv="); Serial.println(derivada);
        delay(50);

        if (getEncoderCentimeters() >= 30.0f) {
          motorStop();
          pararBuscandoTerminado = true;
          Serial.println("[PARAR_BLINKEAR] Avance completado, entrando en bucle de parpadeo infinito.");
        }
      }
    } else {
      // Bucle infinito de parpadeo del LED
      static unsigned long lastBlink = 0;
      static bool ledState = false;
      unsigned long now = millis();
      if (now - lastBlink >= 500) {
        lastBlink = now;
        ledState = !ledState;
        digitalWrite(LedPin, ledState ? HIGH : LOW);
      }
      delay(10);
    }
  }
}