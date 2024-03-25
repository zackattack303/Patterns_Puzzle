/*  REQUIREMENTS - https://docs.google.com/spreadsheets/d/1hCcGfK74lSqU0hnOM8231LkGP0c11GPtNVbxKyvhFn4/edit?usp=drive_link

Reset:					
All white lights are set to OFF					
The red light loop is set to ON					
Delay for 2 seconds					
The red light loop is set to OFF					
Proceed to Level 1					
					
For all Levels:					
All 9 sensor inputs are continuously checked 					
When an input is detected it is logged, in order, in a received array					
When an input is detected the corresponding white light to that input is set to OFF					
An individual input may only be logged once per level					
					
Level 1:					
All white lights are set to ON					
The expected array for Level 1 is '1-2-3'					
Once 3 inputs have been logged the received array is compared to the expected array 					
If the received array matches the expected array proceed to Level 2					
If the received array does not match the expected array proceed to Reset					
					
Level 2:					
All white lights are set to ON					
The expected array for Level 1 is '2-4-6-8'					
Once 4 inputs have been logged the received array is compared to the expected array 					
If the received array matches the expected array proceed to Level 3					
If the received array does not match the expected array proceed to Reset					
					
Level 3:					
All white lights are set to ON					
The expected array for Level 1 is '1-3-5-7-9'					
Once 5 inputs have been logged the received array is compared to the expected array 					
If the received array matches the expected array proceed to Level 4					
If the received array does not match the expected array proceed to Reset					
					
Level 4:					
All white lights are set to ON					
The expected array for Level 1 is '9-8-7-6-5-4'					
Once 6 inputs have been logged the received array is compared to the expected array 					
If the received array matches the expected array proceed to Game Win					
If the received array does not match the expected array proceed to Reset					
					
Game Win:					
All white lights are set ON for .3s then set OFF for .3s. This is repeated 5 times					
All white lights are set to OFF					
The Patterns_Puzzle_Win signal is set to ON for .5s					
The Patterns_Puzzle_Win signal is set of OFF					
The program goes into an indefinite wait and must be reset externally					

Outputs:	
	Patterns_Puzzle_Win - pin29
	White light 1 - pin31
	White light 2 - pin33
	White light 3 - pin35
	White light 4 - pin37
	White light 5 - pin39
	White light 6 - pin41
	White light 7 - pin43
	White light 8 - pin45
	White light 9 - pin47
	Red lights - pin49

 Inputs:	
	Sensor 1 - pin48
	Sensor 2 - pin46
	Sensor 3 - pin44
	Sensor 4 - pin42
	Sensor 5 - pin40
	Sensor 6 - pin38
	Sensor 7 - pin36
	Sensor 8 - pin34
	Sensor 9 - pin32
*/

#include <avr/wdt.h>

const int INPUT_PINS[] = {32, 34, 36, 38, 40, 42, 44, 46, 48}; // Sensors 1-9
const int RED_LIGHTS_PIN = 49; // Red lights string
const int WHITE_LIGHT_PINS[] = {31, 33, 35, 37, 39, 41, 43, 45, 47}; // White lights tied to each input
const int PATTERNS_PUZZLE_WIN_PIN = 29; // Pin to send winning signal to COGS

/*
An array for each level, the first entry being the number of entries to watch for `N`,
followed by `N` integers representing the inputs necessary for success. Padded out
with -1's
*/
const int levels[4][7] = {
  {3, 1, 2, 3, -1, -1, -1},
  {4, 2, 4, 6, 8, -1, -1},
  {5, 1, 3, 5, 7, 9, -1},
  {6, 9, 8, 7, 6, 5, 4}
};

// The current level, zero indexed
int currentLevel = 0;

/*
Record of if an input has already been received to ignore 
previously seen inputs
*/
bool pressed[] = {false, false, false, false, false, false, false, false, false};

// The inputs we have seen, in order
int encounterOrder[] = {-1, -1, -1, -1, -1, -1, -1, -1, -1};
int encounterIdx = 0;


void setup() {
  Serial.begin(9600);

  pinMode(RED_LIGHTS_PIN, OUTPUT);

  for (int i = 0; i < 9; i++) {
    pinMode(INPUT_PINS[i], INPUT_PULLUP);
    pinMode(WHITE_LIGHT_PINS[i], OUTPUT);
    digitalWrite(WHITE_LIGHT_PINS[i], HIGH); // Turn on all white lights at the start
  }
}

void loop() {
  checkInputs();
}

void gameWin() {
  for (int i = 0; i < 5; i++) {
    for (size_t j = 0; j < 9; j++){
      digitalWrite(WHITE_LIGHT_PINS[j], HIGH);
    }
        
    delay(300);
    for (size_t j = 0; j < 9; j++){
      digitalWrite(WHITE_LIGHT_PINS[j], LOW);
    }
    delay(300);
  }
  digitalWrite(PATTERNS_PUZZLE_WIN_PIN, HIGH);
  delay(500);
  digitalWrite(PATTERNS_PUZZLE_WIN_PIN, LOW);
  while (true) {
    // freeze program here until externally reset with RESET signal from COGS)
  }
} // end gameWin

void resetGame() {
  resetGameState();

  for (size_t j = 0; j < 9; j++){
      digitalWrite(WHITE_LIGHT_PINS[j], LOW);
    }
  digitalWrite(RED_LIGHTS_PIN, HIGH);
  delay(2000); // Keep the red lights on for 2 seconds
  digitalWrite(RED_LIGHTS_PIN, LOW);
  fullReset();
} // end resetGame
 
void fullReset() {
  // Perform a full reset using the watchdog timer reset
  wdt_enable(WDTO_15MS); // Enable the watchdog timer with a 15ms timeout
  while (true) {
    // Wait for the watchdog timer to trigger the reset
  } // end fullReset
}

void checkInputs() {
  int limit = levels[currentLevel][0];
  int remaining = limit;
  for (size_t i = 0; i < 9; i++) {
    if (pressed[i]) {
      remaining--;
    }    
  }

  if (remaining <= 0) {
    // check and reset

    /*
    remaining <= 0 means we have gotten at least as many as inputs as are required to 
    sovle the puzzle. It doesn't matter if we received more than N inputs as we will 
    only be checking the first N inputs we observed and any extras will be dropped.
    */

   for (size_t i = 0; i < limit; i++) {
    // Here we need to compare the inputs recorded against the inputs expected, in order
    int expected = levels[currentLevel][i + 1]; // offset by one since the first item is the limit
    int actual = encounterOrder[i];

    if (expected != actual) {
      // A mismatch was encountered, reset and return  
      resetGame();
    } 
   }

   // Getting to this point means up the limit, the inputs were received in the correct order
   // progress the game

    completeLevel();
  } else {
    // There are still inputs remaining, get updates
    // Up to 9 for each input
    for (size_t i = 0; i < 9; i++) {
      int sensorValue = digitalRead(INPUT_PINS[i]);

      if (sensorValue == HIGH && !pressed[i]) {
        // Sensor is triggered and we have not previously seen it
        Serial.println("Input " + String(i + 1) + " received for level: " + String(currentLevel + 1));

        // record the index of the input
        encounterOrder[encounterIdx++] = i;

        // Mark it as seen
        pressed[i] = true;

        // Turn off the light
        digitalWrite(WHITE_LIGHT_PINS[i], LOW);
      } else if (sensorValue == HIGH && pressed[i]) {
        // Sensor is triggered and we have already seen it
        // do nothing
      } else if (sensorValue == LOW) {
        // Sensor is not triggered, do nothing
      }
    } 
  }  
}

void resetGameState() {
  for (size_t i = 0; i < 9; i++) {
    // reset the "seen" status of every sensor
    pressed[i] = false;

    // reset the record of order each sensor is seen
    encounterOrder[i] = -1;

    // reset the index of encounter back to the start
    encounterIdx = 0;

    // Turn on all the white lights
    digitalWrite(WHITE_LIGHT_PINS[i], HIGH);
  }  
}

void completeLevel() {
  // Move to the next level
  currentLevel++;

  // Check for game over condition
  if (currentLevel >= 4) {
    Serial.println("Level is greater than 4, ending game");
    gameWin();
  } else {
    // reset the level to ensure all inputs are zero'd out to handle the new level
    resetGameState();
  }  
}