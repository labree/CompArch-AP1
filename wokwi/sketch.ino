/*
 * Sistema de Detecção de Alvo
 * 
 * Autores:
 * Enrique Mari Clavecilla Labre, 202408380501, TA
 * Bernardo Casanovas Linhares, 202407085879, TA
 * Sabrina Herdy Marenga, 202407095951, TP
 * Anderson Lima de Araujó Junior, 202408573421, TA
 */

// Pinos do sensor e buzzer
const int TRIG_PIN = 9;
const int ECHO_PIN = 10;
const int BUZZER_PIN = 8;

// Pinos dos LEDs de distância
const int LED_PINS[] = {2, 3, 4, 5, 6};
const int NUM_LEDS = 5;

// Chaves de controle
const int SWITCH1_PIN = 11;
const int SWITCH2_PIN = 12;

// LEDs indicadores de chave
const int SWITCH1_LED = 7;
const int SWITCH2_LED = 13;

// Limiares em cm
const int DISTANCE_4M = 400;
const int DISTANCE_2M = 200;
const int DISTANCE_1M = 100;

// Temporizações (em ms)
const unsigned long BLINK_SLOW = 1000;
const unsigned long BLINK_FAST = 500;

// Estado do sistema
bool systemEnabled = false;
bool soundEnabled = true;

// Controle de tempo
unsigned long lastLEDToggle = 0;
unsigned long lastBuzzerToggle = 0;
int currentLED = 0;
bool buzzerState = false;

void setup() {
  Serial.begin(9600);
  Serial.println("Sistema de Detecção de Alvo Inicializado");

  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(BUZZER_PIN, OUTPUT);

  for (int i = 0; i < NUM_LEDS; i++) {
    pinMode(LED_PINS[i], OUTPUT);
    digitalWrite(LED_PINS[i], LOW);
  }

  pinMode(SWITCH1_PIN, INPUT_PULLUP);
  pinMode(SWITCH2_PIN, INPUT_PULLUP);
  pinMode(SWITCH1_LED, OUTPUT);
  pinMode(SWITCH2_LED, OUTPUT);
}

float getDistance() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  float duration = pulseIn(ECHO_PIN, HIGH);
  float distance = duration * 0.034 / 2; // em cm
  return distance;
}

void handleLEDs(float distance) {
  unsigned long interval = (distance > DISTANCE_2M) ? BLINK_SLOW : BLINK_FAST;
  int activeLEDs = (distance > DISTANCE_2M) ? 3 : 4;

  // Todos os LEDs desligados por padrão
  for (int i = 0; i < NUM_LEDS; i++) {
    digitalWrite(LED_PINS[i], LOW);
  }

  if (distance > DISTANCE_4M) return;

  if (distance > DISTANCE_1M) {
    // Piscar LEDs alternadamente
    if (millis() - lastLEDToggle >= interval) {
      lastLEDToggle = millis();
      for (int i = 0; i < activeLEDs; i++) {
        digitalWrite(LED_PINS[i], LOW);
      }
      digitalWrite(LED_PINS[currentLED], HIGH);
      currentLED = (currentLED + 1) % activeLEDs;
    }
  } else {
    // Abaixo de 1m: todos acesos
    for (int i = 0; i < NUM_LEDS; i++) {
      digitalWrite(LED_PINS[i], HIGH);
    }
  }
}

void handleBuzzer(float distance) {
  if (!soundEnabled || distance > DISTANCE_4M) {
    noTone(BUZZER_PIN);
    return;
  }

  if (distance > DISTANCE_2M) {
    // Som intermitente lento (2m - 4m)
    if (millis() - lastBuzzerToggle >= BLINK_SLOW) {
      lastBuzzerToggle = millis();
      buzzerState = !buzzerState;
      if (buzzerState) tone(BUZZER_PIN, 1000);
      else noTone(BUZZER_PIN);
    }
  } else if (distance > DISTANCE_1M) {
    // Som intermitente rápido (1m - 2m)
    if (millis() - lastBuzzerToggle >= BLINK_FAST) {
      lastBuzzerToggle = millis();
      buzzerState = !buzzerState;
      if (buzzerState) tone(BUZZER_PIN, 1000);
      else noTone(BUZZER_PIN);
    }
  } else {
    // Som contínuo (< 1m)
    tone(BUZZER_PIN, 1000);
  }
}

void loop() {

  float distance = getDistance();

  Serial.print("Distância: ");
  Serial.print(distance);
  Serial.println(" cm");

  handleLEDs(distance);
  handleBuzzer(distance);

  delay(100);
}
