#include <avr/wdt.h>

const int INPUT_PINS[] = {2, 3, 4, 5};
const int GREEN_LIGHT_PIN = 7;
const int RED_LIGHT_PIN = 8;
const int WHITE_LIGHT_PINS[] = {9, 10, 11, 12}; // White lights tied to each input
const int LIGHT_OFF_DURATION = 500;
const int RED_LIGHT_DURATION = 5000; // 5 seconds for red light duration
const int SEQUENCE1[] = {1, 2, 3, 4};
const int SEQUENCE2[] = {4, 3, 2, 1};
const int SEQUENCE3[] = {1, 3, 2, 4};
const int SEQUENCE4[] = {4, 2, 3, 1};
const int SEQUENCE_TIMEOUT = 10000; // 10 seconds timeout for each sequence

int inputOrder[4] = {0, 0, 0, 0};
bool greenLightOn = false; // Variable to track if the green light is on
unsigned long sequenceStartTime = 0; // Variable to store the start time of each sequence
bool allSequencesCompleted = false; // Variable to track if all sequences have been completed

void setup() {
  Serial.begin(9600);

  pinMode(GREEN_LIGHT_PIN, OUTPUT);
  pinMode(RED_LIGHT_PIN, OUTPUT);

  for (int i = 0; i < 4; i++) {
    pinMode(INPUT_PINS[i], INPUT_PULLUP);
    pinMode(WHITE_LIGHT_PINS[i], OUTPUT);
    digitalWrite(WHITE_LIGHT_PINS[i], HIGH); // Turn on all white lights at the start
  }
}

void loop() {
  // Check for sequence 1
  if (checkOrderWithTimeout(SEQUENCE1, 1)) {
    // Correct order detected
    // Perform the required actions (blink green light, turn off white lights, etc.)
  }

  // Check for sequence 2
  if (checkOrderWithTimeout(SEQUENCE2, 2)) {
    // Correct order detected
    // Perform the required actions (blink green light, turn off white lights, etc.)
  }

  // Check for sequence 3
  if (checkOrderWithTimeout(SEQUENCE3, 3)) {
    // Correct order detected
    // Perform the required actions (blink green light, turn off white lights, etc.)
  }

  // Check for sequence 4
  if (checkOrderWithTimeout(SEQUENCE4, 4)) {
    // Correct order detected
    // Perform the required actions (blink green light, turn off white lights, etc.)
    allSequencesCompleted = true; // Set the flag to true when sequence 4 is completed
  }

  // Turn on the green light only when all sequences are completed
  if (allSequencesCompleted && !greenLightOn) {
    greenLightOn = true;
    digitalWrite(GREEN_LIGHT_PIN, HIGH);
    turnOffWhiteLights(); // Turn off all white lights
    // Program freezes indefinitely
    while (true) {
      // Do nothing or add any additional logic if needed
    }
  }
}

bool checkOrderWithTimeout(const int order[], int greenLightBlinks) {
  int inputIndex = 0;
  sequenceStartTime = millis(); // Record the start time for the current sequence

  // Wait until all inputs are detected, or timeout occurs
  while (inputIndex < 4 && (millis() - sequenceStartTime) < SEQUENCE_TIMEOUT) {
    for (int i = 0; i < 4; i++) {
      if (digitalRead(INPUT_PINS[i]) == LOW && inputOrder[i] == 0) {
        // Input detected for the first time, log the order
        inputOrder[i] = inputIndex + 1;
        inputIndex++;

        Serial.print("Input Detected on Pin ");
        Serial.println(INPUT_PINS[i]);

        // Turn off the corresponding white light
        digitalWrite(WHITE_LIGHT_PINS[i], LOW);
      }
    }
  }

  // Check if the input order is correct and within the time limit
  bool correctOrder = true;
  for (int i = 0; i < 4; i++) {
    if (inputOrder[i] != order[i]) {
      correctOrder = false;
      break;
    }
  }

  if (correctOrder && (millis() - sequenceStartTime) < SEQUENCE_TIMEOUT) {
    // Turn on the corresponding light based on the result
    blinkGreenLight(greenLightBlinks);
    reset();
    delay(1000);
    return true;
  } else {
    // Timeout or incorrect order detected, flash red light and perform a full reset
    flashRedLight();
    fullReset();
    return false;
  }
}

void blinkGreenLight(int blinks) {
  if (greenLightOn) {
    for (int i = 0; i < blinks; i++) {
      digitalWrite(GREEN_LIGHT_PIN, HIGH);
      delay(LIGHT_OFF_DURATION);
      digitalWrite(GREEN_LIGHT_PIN, LOW);
      delay(LIGHT_OFF_DURATION);
    }
  }
}

void flashRedLight() {
  digitalWrite(RED_LIGHT_PIN, HIGH);
  delay(RED_LIGHT_DURATION); // Keep the red light on for 5 seconds
  digitalWrite(RED_LIGHT_PIN, LOW);
}

void reset() {
  // Reset variables for the next attempt
  for (int i = 0; i < 4; i++) {
    inputOrder[i] = 0;
  }

  // Restore the state of inputs and turn on white lights
  for (int i = 0; i < 4; i++) {
    pinMode(INPUT_PINS[i], INPUT_PULLUP);
    digitalWrite(WHITE_LIGHT_PINS[i], HIGH);
  }

  // Turn off green light and reset the variable
  digitalWrite(GREEN_LIGHT_PIN, LOW);
  greenLightOn = false;

  Serial.println("Resetting.");
}

void fullReset() {
  // Perform a full reset using the watchdog timer reset
  wdt_enable(WDTO_15MS); // Enable the watchdog timer with a 15ms timeout
  while (true) {
    // Wait for the watchdog timer to trigger the reset
  }
}

void turnOffWhiteLights() {
  // Turn off all white lights
  for (int i = 0; i < 4; i++) {
    digitalWrite(WHITE_LIGHT_PINS[i], LOW);
  }
}
