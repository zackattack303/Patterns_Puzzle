// Inputs
const int INPUT_PINS[] = {2, 3, 4, 5, 6};

// Outputs
const int GREEN_LIGHT = 7;
const int RED_LIGHT = 8;
const int SUCCESS_MARKERS[] = {9, 10, 11, 12};

// Game Progression
const int levels[4][4] = {
  {2, 3, 4, 6},
  {2, 3, 5, 6},
  {2, 3, 4, 5},
  {2, 4, 5, 6}
};
int currentLevel = 0;
bool gameComplete = false;
bool presses[] = {false, false, false, false};


void setup() {
  Serial.begin(9600);
  pinMode(GREEN_LIGHT, OUTPUT);
  pinMode(RED_LIGHT, OUTPUT);

  for (int i = 0; i < 5; i++) {
    pinMode(INPUT_PINS[i], INPUT_PULLUP);
  }

  initSuccessMarkers();
}

void loop() {
  if (!gameComplete) {
    // Get input from players
    checkButtonPress();

    // Progress Level
    bool levelComplete = checkLevelComplete();
    if (levelComplete && currentLevel == 3) {
      gameComplete = true;
      return;
    } else if (levelComplete) {
      // Progress to the next level
      currentLevel++;
      flashGreen(4);
      Serial.println("Progressing to new level: " + String(currentLevel + 1));
      
      // Reset game state
      initSuccessMarkers();
      for (size_t i = 0; i < 4; i++) {
        presses[i] = false;
      }
    }    
  } else {
    digitalWrite(GREEN_LIGHT, HIGH);
    Serial.println("All levels completed, Game Over");
    delay(500);  
  }
  
  
}

void checkButtonPress() {
  for (size_t i = 0; i < 5; i++){
    if (digitalRead(INPUT_PINS[i]) == LOW) {
      delay(50); //debounce
      if (digitalRead(INPUT_PINS[i]) == LOW) {
        Serial.println("Input " + String(i + 1) + " received for level: " + String(currentLevel + 1));

        for (size_t j = 0; j < 4; j++) {
          if (INPUT_PINS[i] == levels[currentLevel][j]) {
            // Is a required input
            presses[j] = true;
            lightRemainingInputs();
          }          
        }
      }
    }    
  }  
}

void lightRemainingInputs() {
  int counter = 0;
  for (size_t i = 0; i < 4; i++) {
    // count the number remaining
  	counter += presses[i] == true;
  }
  
  for (size_t i = 0; i < counter; i++) {
    digitalWrite(SUCCESS_MARKERS[4 - 1 - i], LOW);
  }
}

bool checkLevelComplete() {
  for (size_t i = 0; i < 4; i++){
    if (!presses[i]) {
      return false;
    }
  }
  
  Serial.println("---> Level Complete");
  return true;
}

void initSuccessMarkers() {
  for (int i = 0; i < 4; i++) {
    pinMode(SUCCESS_MARKERS[i], OUTPUT);
    digitalWrite(SUCCESS_MARKERS[i], HIGH);
  }
}

void flashGreen(int blinks) {
    for (int i = 0; i < blinks; i++) {
      digitalWrite(GREEN_LIGHT, HIGH);
      delay(500);
      digitalWrite(GREEN_LIGHT, LOW);
      delay(500);
    }
}