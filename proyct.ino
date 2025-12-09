#include <Servo.h>

// =====================
//        TENDEDERO
// =====================
Servo tendedero;
int pinServo = 9;
int pinSensor = 6;

// =====================
//        GARAJE
// =====================
Servo garaje;
int pinServoGaraje = 5;

#define TRIG 7
#define ECHO 8

// =====================
//      LDR + LEDs
// =====================
const int ldrPin = A0;
const int led1Pin = 3;
const int led2Pin = 4;
const int ledExtra = 12;

const int LDR_THRESHOLD = 600;
const int EXTRA_LED_THRESHOLD = 300;
const unsigned long STABLE_MS = 150;

int ldrSamples[5];
int sampleIndex = 0;
bool ldrReady = false;

bool ledsOn = false;
unsigned long lastChangeTime = 0;
bool lastBrightState = false;

// =====================
//  GARAJE HISTERESIS
// =====================
bool garajeAbierto = false;
const int DIST_ABRIR = 20;
const int DIST_CERRAR = 30;

// =====================
// CONTROL IMPRESIONES
// =====================
unsigned long lastPrint = 0;
const unsigned long PRINT_INTERVAL = 2000; // 2 segundos entre impresiones

// Variables para impresión
int lluvia;
long distancia;
int ldrValue;


// =====================
//        SETUP
// =====================
void setup() {
  Serial.begin(9600);

  tendedero.attach(pinServo);
  garaje.attach(pinServoGaraje);

  pinMode(pinSensor, INPUT);
  pinMode(TRIG, OUTPUT);
  pinMode(ECHO, INPUT);

  pinMode(led1Pin, OUTPUT);
  pinMode(led2Pin, OUTPUT);
  pinMode(ledExtra, OUTPUT);

  for (int i = 0; i < 5; i++) ldrSamples[i] = 0;

  Serial.println("====================================");
  Serial.println(" SISTEMA AUTOMATIZADO INICIADO");
  Serial.println("====================================\n");
}


// =====================
//   LECTURA SUAVIZADA
// =====================
int readLdrSmoothed() {
  ldrSamples[sampleIndex++] = analogRead(ldrPin);
  if (sampleIndex >= 5) {
    sampleIndex = 0;
    ldrReady = true;
  }

  long sum = 0;
  for (int i = 0; i < 5; i++) sum += ldrSamples[i];

  return sum / 5;
}


// =====================
//   DISTANCIA HC-SR04
// =====================
long medirDistancia() {
  digitalWrite(TRIG, LOW);
  delayMicroseconds(3);
  digitalWrite(TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG, LOW);

  long duracion = pulseIn(ECHO, HIGH, 30000);
  if (duracion == 0) return 999;

  return duracion * 0.034 / 2;
}


// =====================
//         LOOP
// =====================
void loop() {

  // --------------------------------------------------------
  // Tendedero
  // --------------------------------------------------------
  lluvia = digitalRead(pinSensor);
  if (lluvia == HIGH) {
    tendedero.write(0);
  } else {
    tendedero.write(90);
  }

  // --------------------------------------------------------
  // Garaje con histeresis
  // --------------------------------------------------------
  distancia = medirDistancia();

  if (!garajeAbierto && distancia <= DIST_ABRIR) {
    garaje.write(90);
    garajeAbierto = true;
  }
  else if (garajeAbierto && distancia >= DIST_CERRAR) {
    garaje.write(0);
    garajeAbierto = false;
  }

  // --------------------------------------------------------
  // LDR + LEDs
  // --------------------------------------------------------
  ldrValue = readLdrSmoothed();

  if (ldrReady) {
    bool isBright = (ldrValue > LDR_THRESHOLD);

    if (isBright != lastBrightState) {
      lastChangeTime = millis();
      lastBrightState = isBright;
    }
    else if (millis() - lastChangeTime >= STABLE_MS) {

      if (isBright && ledsOn) {
        digitalWrite(led1Pin, LOW);
        digitalWrite(led2Pin, LOW);
        ledsOn = false;
      }
      else if (!isBright && !ledsOn) {
        digitalWrite(led1Pin, HIGH);
        digitalWrite(led2Pin, HIGH);
        ledsOn = true;
      }
    }

    // LED extra
    if (ldrValue < EXTRA_LED_THRESHOLD)
      digitalWrite(ledExtra, HIGH);
    else
      digitalWrite(ledExtra, LOW);
  }

  // --------------------------------------------------------
  // IMPRESIONES CADA 2 SEGUNDOS
  // --------------------------------------------------------
  if (millis() - lastPrint >= PRINT_INTERVAL) {
    lastPrint = millis();

    Serial.println("============== ESTADO ==============");

    // Tendedero
    Serial.print("[TENDEDERO] Sensor lluvia: ");
    Serial.println(lluvia == HIGH ? "LLUVIA (metiendo ropa)" : "SIN LLUVIA (sacando ropa)");

    // Garaje
    Serial.print("[GARAJE] Distancia: ");
    Serial.print(distancia);
    Serial.print(" cm → Estado: ");
    Serial.println(garajeAbierto ? "ABIERTO" : "CERRADO");

    // LDR
    Serial.print("[LDR] Valor suavizado: ");
    Serial.println(ldrValue);

    // LEDs
    Serial.print("[LEDS PRINCIPALES] ");
    Serial.println(ledsOn ? "ENCENDIDOS" : "APAGADOS");

    // LED extra
    Serial.print("[LED EXTRA] ");
    Serial.println(ldrValue < EXTRA_LED_THRESHOLD ? "ENCENDIDO" : "APAGADO");

    Serial.println("====================================\n");
  }
}