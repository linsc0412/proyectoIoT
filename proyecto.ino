#include <Servo.h>

// ==== TENDEDERO (original) ====
Servo tendedero;
int pinServo = 9;
int pinSensor = 6;  // sensor de lluvia (digital)

// ==== GARAJE (servo) ====
Servo garaje;
int pinServoGaraje = 5;

// ==== HC-SR04 ====
#define TRIG 10
#define ECHO 11

// ==== LEDs y LDR ====
const int led1Pin = 3;
const int led2Pin = 4;
const int ldrPin = A0;

// Parámetros LDR (ajusta según tu entorno)
const int LDR_THRESHOLD = 600;   // umbral (0..1023). Subilo o bajalo según pruebas
const unsigned long STABLE_MS = 150; // tiempo mínimo estable antes de cambiar estado

// Variables para debounce/histeresis y promedio
int ldrSamples[5];
int sampleIndex = 0;
bool ledsOn = false;
unsigned long lastChangeTime = 0;
bool lastBrightState = false; // true = bright (flash), false = dark

void setup() {
  Serial.begin(9600);

  // Tendedero
  tendedero.attach(pinServo);
  pinMode(pinSensor, INPUT);

  // Garaje
  garaje.attach(pinServoGaraje);

  // HC-SR04
  pinMode(TRIG, OUTPUT);
  pinMode(ECHO, INPUT);

  // LEDs
  pinMode(led1Pin, OUTPUT);
  pinMode(led2Pin, OUTPUT);
  digitalWrite(led1Pin, LOW);
  digitalWrite(led2Pin, LOW);

  // Init samples
  for (int i = 0; i < 5; i++) ldrSamples[i] = 0;

  Serial.println("Sistema iniciado...");
}

int readLdrSmoothed() {
  // Lectura y promedio simple de 5 muestras
  ldrSamples[sampleIndex++] = analogRead(ldrPin);
  if (sampleIndex >= 5) sampleIndex = 0;

  long sum = 0;
  for (int i = 0; i < 5; i++) sum += ldrSamples[i];
  return (int)(sum / 5);
}

void loop() {
  // -------------------------
  // LÓGICA DEL TENDEDERO (igual que antes)
  // -------------------------
  int lectura = digitalRead(pinSensor);
  if (lectura == LOW) {  
    // LOW = lluvia (según tu módulo)
    Serial.println("Lluvia detectada → Metiendo la ropa...");
    tendedero.write(0);   // dentro del techo
  } else {
    Serial.println("Sin lluvia → Sacando la ropa...");
    tendedero.write(90);  // afuera
  }

  // -------------------------
  // LÓGICA DEL HC-SR04 (garaje)
  // -------------------------
  long duracion, distancia;
  digitalWrite(TRIG, LOW);
  delayMicroseconds(5);
  digitalWrite(TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG, LOW);

  duracion = pulseIn(ECHO, HIGH, 30000); // timeout 30ms
  if (duracion == 0) {
    distancia = 999; // sin lectura (muy lejos o sin eco)
  } else {
    distancia = duracion * 0.034 / 2;
  }

  Serial.print("Distancia al 'carro': ");
  Serial.print(distancia);
  Serial.println(" cm");

  if (distancia > 0 && distancia <= 20) {
    Serial.println("Carro detectado → Abriendo garaje");
    garaje.write(90);
  } else {
    Serial.println("Sin carro → Garaje cerrado");
    garaje.write(0);
  }

  // -------------------------
  // LÓGICA DEL LDR y LEDs
  // -------------------------
  int ldrValue = readLdrSmoothed();
  Serial.print("LDR (prom): ");
  Serial.println(ldrValue);

  bool isBright = (ldrValue > LDR_THRESHOLD); // true si hay flash/mucha luz

  // Si el estado cambió, esperar STABLE_MS para aplicar (debounce)
  if (isBright != lastBrightState) {
    lastChangeTime = millis();
    lastBrightState = isBright;
  } else {
    if (millis() - lastChangeTime >= STABLE_MS) {
      // estado estable -> aplicar cambio real
      bool shouldTurnOffLEDs = isBright; // si está brillante (flash) => LEDs apagadas
      if (shouldTurnOffLEDs && ledsOn) {
        // apagar leds
        digitalWrite(led1Pin, LOW);
        digitalWrite(led2Pin, LOW);
        ledsOn = false;
        Serial.println("Leds apagadas por flash/alto brillo");
      } else if (!shouldTurnOffLEDs && !ledsOn) {
        // encender leds (simular noche)
        digitalWrite(led1Pin, HIGH);
        digitalWrite(led2Pin, HIGH);
        ledsOn = true;
        Serial.println("Leds encendidas (sin flash)");
      }
    }
  }

  delay(200); // loop relativamente rápido pero no excesivo
}
