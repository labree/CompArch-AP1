/*
 * Sistema de Detecção de Alvo
 * 
 * Autores:
 * Enrique Mari Clavecilla Labre, 202408380501
 * [Nome e matrícula]
 * [Nome e matrícula]
 * [Nome e matrícula]
 */

// Pin Definitions
const int TRIG_PIN = 9;    // Ultrasonic sensor trigger pin
const int ECHO_PIN = 10;   // Ultrasonic sensor echo pin
const int BUZZER_PIN = 8;  // Buzzer pin

// LED pins for distance indication
const int LED_PINS[] = {2, 3, 4, 5, 6};  // 5 LEDs for different distance ranges
const int NUM_LEDS = 5;

// Switch pins (using jumpers)
const int SWITCH1_PIN = 11;
const int SWITCH2_PIN = 12;

// Status LED pins for switches
const int SWITCH1_LED = 13;
const int SWITCH2_LED = 14;

// Distance thresholds (in centimeters)
const int DISTANCE_4M = 400;
const int DISTANCE_2M = 200;
const int DISTANCE_1M = 100;

// Timing constants
const int BLINK_SLOW = 1000;    // 1 second for 2-4m range
const int BLINK_FAST = 500;     // 0.5 seconds for 1-2m range
const int BUZZER_SLOW = 1000;   // 1 second for 2-4m range
const int BUZZER_FAST = 500;    // 0.5 seconds for 1-2m range

// System state
bool systemEnabled = false;
bool soundEnabled = true;

void setup() {
  // Initialize serial communication
  Serial.begin(9600);
  Serial.println("Sistema de Detecção de Alvo Inicializado");
  
  // Configure pins
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  
  // Configure LED pins
  for (int i = 0; i < NUM_LEDS; i++) {
    pinMode(LED_PINS[i], OUTPUT);
    digitalWrite(LED_PINS[i], LOW);
  }
  
  // Configure switch pins
  pinMode(SWITCH1_PIN, INPUT_PULLUP);
  pinMode(SWITCH2_PIN, INPUT_PULLUP);
  pinMode(SWITCH1_LED, OUTPUT);
  pinMode(SWITCH2_LED, OUTPUT);
}

// Function to read distance from ultrasonic sensor
float getDistance() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  
  float duration = pulseIn(ECHO_PIN, HIGH);
  float distance = duration * 0.034 / 2;  // Convert to centimeters
  
  return distance;
}

// Function to handle LED patterns based on distance
void handleLEDs(float distance) {
  // Turn off all LEDs first
  for (int i = 0; i < NUM_LEDS; i++) {
    digitalWrite(LED_PINS[i], LOW);
  }
  
  if (distance > DISTANCE_4M) {
    // No LEDs for distance > 4m
    return;
  } else if (distance > DISTANCE_2M) {
    // 3 LEDs blinking for 2-4m
    for (int i = 0; i < 3; i++) {
      digitalWrite(LED_PINS[i], !digitalRead(LED_PINS[i]));
    }
  } else if (distance > DISTANCE_1M) {
    // 4 LEDs blinking faster for 1-2m
    for (int i = 0; i < 4; i++) {
      digitalWrite(LED_PINS[i], !digitalRead(LED_PINS[i]));
    }
  } else {
    // 5 LEDs on continuously for < 1m
    for (int i = 0; i < NUM_LEDS; i++) {
      digitalWrite(LED_PINS[i], HIGH);
    }
  }
}

// Function to handle buzzer based on distance
void handleBuzzer(float distance) {
  if (!soundEnabled) return;
  
  if (distance > DISTANCE_4M) {
    noTone(BUZZER_PIN);
  } else if (distance > DISTANCE_2M) {
    // Intermittent beep for 2-4m
    static unsigned long lastToggle = 0;
    if (millis() - lastToggle >= BUZZER_SLOW) {
      lastToggle = millis();
      if (digitalRead(BUZZER_PIN)) {
        noTone(BUZZER_PIN);
      } else {
        tone(BUZZER_PIN, 1000);
      }
    }
  } else if (distance > DISTANCE_1M) {
    // Faster beep for 1-2m
    static unsigned long lastToggle = 0;
    if (millis() - lastToggle >= BUZZER_FAST) {
      lastToggle = millis();
      if (digitalRead(BUZZER_PIN)) {
        noTone(BUZZER_PIN);
      } else {
        tone(BUZZER_PIN, 1000);
      }
    }
  } else {
    // Continuous tone for < 1m
    tone(BUZZER_PIN, 1000);
  }
}

// Function to check and update system state based on switches
void checkSystemState() {
  bool switch1 = !digitalRead(SWITCH1_PIN);  // Inverted because of INPUT_PULLUP
  bool switch2 = !digitalRead(SWITCH2_PIN);
  
  // Update switch indicator LEDs
  digitalWrite(SWITCH1_LED, switch1);
  digitalWrite(SWITCH2_LED, switch2);
  
  // Update system state
  if (switch1 == 0 && switch2 == 0) {
    systemEnabled = false;
    soundEnabled = false;
  } else if (switch1 == 1 && switch2 == 1) {
    systemEnabled = true;
    soundEnabled = true;
  } else {
    systemEnabled = true;
    soundEnabled = false;
  }
}

void loop() {
  // Check system state
  checkSystemState();
  
  if (systemEnabled) {
    // Get distance
    float distance = getDistance();
    
    // Print distance to serial monitor
    Serial.print("Distância: ");
    Serial.print(distance);
    Serial.println(" cm");
    
    // Handle LEDs and buzzer based on distance
    handleLEDs(distance);
    handleBuzzer(distance);
  } else {
    // System is disabled, turn off all outputs
    for (int i = 0; i < NUM_LEDS; i++) {
      digitalWrite(LED_PINS[i], LOW);
    }
    noTone(BUZZER_PIN);
  }
  
  // Small delay to prevent too frequent readings
  delay(100);
} 
