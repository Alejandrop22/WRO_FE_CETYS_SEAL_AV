#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_VL53L1X.h>
#include "soc/soc.h"
#include "soc/rtc_cntl_reg.h"

// ================== PINES ==================
#define XSHUT_RIGHT 15
#define XSHUT_LEFT  13
#define LedPin 18
#define BotonPin 23
#define EncoderPin 19

// ================== OBJETOS ==================
Adafruit_VL53L1X sensor_right = Adafruit_VL53L1X();
Adafruit_VL53L1X sensor_left  = Adafruit_VL53L1X();

// ================== VARIABLES GLOBALES ==================
volatile long encoderCount = 0;

// Direcciones I2C
#define ADDR_S1 0x30   // Sensor derecho
#define ADDR_S2 0x31   // Sensor izquierdo

// Direcciones de movimiento
#define ADELANTE 1
#define ATRAS    2

// Estados de la máquina
enum Estado {
    BUSCAR_PARED_INICIAL,
    SEGUIR_PARED_DERECHA,
    SEGUIR_PARED_IZQUIERDA,
    GIRO_DERECHA,
    GIRO_IZQUIERDA,
    PARAR_BLINKEAR
};

Estado estado = BUSCAR_PARED_INICIAL;

// Contadores y límites
int giroCount = 0;
const int MAX_GIROS = 12;

// Distancias
float forwardAfterCenterCmForCurrentTurn(int currentTurnIndex) {
    if (currentTurnIndex == 0) {
        return 30.0f;
    } else {
        return 5.0f;
    }
}

float turnDistanceCmForCurrentTurn(int currentTurnIndex) {
    if (currentTurnIndex == 0) {
        return 29.0f;
    } else {
        return 25.0f;
    }
}

// Sensores y estado
bool s_right_ok = false;
bool s_left_ok = false;
bool sensorRightActivo = true;
bool sensorLeftActivo  = true;

uint16_t lastDistR = 9999;
uint16_t lastDistL = 9999;
unsigned long lastUpdateR = 0;
unsigned long lastUpdateL = 0;

const unsigned long DATA_STALE_MS = 150;
const unsigned long INTERVALO_LECTURA = 50;
unsigned long ultimaLectura = 0;
const uint16_t UMBRAL_ESQUINA = 200;

// Motor y servo
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

// PID
float Kp = 0.28f;
float Ki = 0.006f;
float Kd = 0.055f;
float pid_integral = 0.0f;
float pid_prev_error = 0.0f;
unsigned long lastPIDTime = 0;
const float INTEGRAL_MAX = 300.0f;

// Búsqueda inicial
unsigned long buscarStartTime = 0;
bool buscarTimerRunning = false;
const unsigned long BUSCAR_TURN_DELAY_MS = 1000;

// Parar y parpadear
bool pararBuscandoTerminado = false;
bool ultimoLadoDerecha = true;

// ================== FUNCIONES ==================
// Interrupción del encoder
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

// ================== SETUP ==================
void setup() {
    WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0);
    Serial.begin(115200);
    delay(100);
    Serial.println("[SETUP] Inicio");

    pinMode(LedPin, OUTPUT);

    Wire.begin(17, 16);
    Serial.println("[SETUP] Wire iniciado");

    // Configurar motor y servo
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

    // Inicializar sensores VL53L1X
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

// ================== LOOP ==================
void loop() {
    if (estado == BUSCAR_PARED_INICIAL) {
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

    // ================== BLOQUE DE GIRO ==================
    if (estado == GIRO_DERECHA || estado == GIRO_IZQUIERDA) {
        const bool giroDerecha = estado == GIRO_DERECHA;
        const char *nombreGiro = giroDerecha ? "[GIRO_DERECHA]" : "[GIRO_IZQUIERDA]";

        digitalWrite(LedPin, HIGH);
        motorStop();
        delay(100);

        motorStop();
        delay(200);

        moverServo(giroDerecha ? ServoMax : ServoMin);

        resetEncoder();
        motorVelocity(VELOCIDAD_BASE - 10, ADELANTE);

        Serial.print(nombreGiro);
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

        giroCount++;
        if (giroCount >= MAX_GIROS) {
            Serial.println("[GIRO] Max giros alcanzado, entrando en modo PARAR_BLINKEAR");
            pararBuscandoTerminado = false;
            resetEncoder();
            estado = PARAR_BLINKEAR;
        } else {
            estado = giroDerecha ? SEGUIR_PARED_DERECHA : SEGUIR_PARED_IZQUIERDA;
            ultimoLadoDerecha = giroDerecha;
            activarSensoresParaLado(giroDerecha);
        }
    }

    // ================== SEGUIMIENTO DE PARED ==================
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