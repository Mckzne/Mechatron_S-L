#include <Adafruit_NeoPixel.h>

#define LED_PIN 6
#define NUM_LEDS 200
#define NUM_SNAKES 6          // Number of snakes
#define SNAKE_LENGTH 8        // Length of each snake in LEDs
#define MAX_BRIGHTNESS 255    // Max brightness per color channel
#define MIN_BRIGHTNESS 30     // Minimum brightness for faint tail LEDs
#define FADE_STEP  (MAX_BRIGHTNESS - MIN_BRIGHTNESS) / (SNAKE_LENGTH - 1)

#define NUMBER_COLOR strip.Color(0, 150, 255) // Water blue for dice number

Adafruit_NeoPixel strip(NUM_LEDS, LED_PIN, NEO_GRB + NEO_KHZ800);

// Segment definitions
int segA[] = {22, 23, 24, 25};
int segB[] = {25, 32, 45, 52, 65};
int segC[] = {104, 91, 84, 72, 65};
int segD[] = {101, 102, 103, 104};
int segE[] = {62, 75, 81, 94, 101};
int segF[] = {22, 35, 42, 55, 62};
int segG[] = {65, 64, 63, 62};

// Segment order paths for digits 0-9
int segmentOrder[10][7] = {
  {0,5,4,3,2,1,-1},    
  {1,2,-1,-1,-1,-1,-1}, 
  {0,1,6,4,3,-1,-1},   
  {0,1,6,2,3,-1,-1},   
  {5,6,1,2,-1,-1,-1},  
  {0,5,6,2,3,-1,-1},   
  {0,5,4,3,2,6,-1},    
  {0,1,2,-1,-1,-1,-1}, 
  {0,5,6,2,3,4,1},     
  {1,2,0,5,6,-1,-1}    
};

// Segment reverse flags for digits 0-9
int segmentReverse[10][7] = {
  {1,0,0,0,0,1,-1}, 
  {0,1,-1,-1,-1,-1,-1}, 
  {0,0,0,0,0,-1,-1},    
  {0,0,1,1,0,-1,-1},    
  {0,1,1,0,-1,-1,-1},   
  {1,0,1,1,0,-1,-1},    
  {1,0,0,0,0,0,-1},     
  {0,0,1,-1,-1,-1,-1},  
  {0,0,0,0,0,0,0},      
  {0,0,0,0,-1,-1,-1}     
};

// Full-cycle uniqueness tracker for dice rolls
bool rolled[6] = {false,false,false,false,false,false};

// Snake paths (0-indexed)
const int snakePaths[NUM_SNAKES][24] = {
  {105, 95, 94, 83, 103, 113, 123, 122, 121, 111, 101, 90},               // Snake 1 (12 LEDs)
  {6, 15, 14, 23, 33, 3, 53, 42, 52, 62, 72, 81, 90},                    // Snake 2 (13 LEDs)
  {36, 35, 34, 44, 54, 63, 71, 82, 93, 102, 113, 124, 125},              // Snake 3 (13 LEDs)
  {1, 11, 22, 33, 34, 35, 45, 54, 65, 66, 75, 85, 84, 83, 82, 81, 90, 101}, // Snake 4 (18 LEDs)
  {100, 91, 82, 73, 65, 55, 54, 53, 42, 33, 23, 24, 25, 15, 5},          // Snake 5 (15 LEDs)
  {121, 111, 101, 90, 80, 71, 63, 54, 55, 45, 35, 34, 33, 32, 21, 11, 2, 3, 4, 5, 6, 7} // Snake 6 (22 LEDs)
};
const int snakePathLengths[NUM_SNAKES] = {12, 13, 13, 18, 15, 22};

// Colors for each snake
const uint8_t baseColors[NUM_SNAKES][3] = {
  {255, 0, 0},     // Red
  {0, 255, 0},     // Green
  {0, 0, 255},     // Blue
  {255, 255, 0},   // Yellow
  {255, 0, 255},   // Magenta
  {0, 255, 255}    // Cyan
};

struct Snake {
  int headIndex;    // Current head position on path
  int direction;    // +1 forward, -1 backward
  bool active;
};

Snake snakes[NUM_SNAKES];

// Helper: scale RGB color by brightness (0-255)
uint32_t colorWithBrightness(uint8_t r, uint8_t g, uint8_t b, uint8_t brightness) {
  uint32_t rr = (uint32_t)r * brightness / 255;
  uint32_t gg = (uint32_t)g * brightness / 255;
  uint32_t bb = (uint32_t)b * brightness / 255;
  return strip.Color(rr, gg, bb);
}

void snakesAnimation(int duration_ms) {
  // Initialize snakes randomly on their paths, random direction
  for (int i = 0; i < NUM_SNAKES; i++) {
    snakes[i].headIndex = random(-SNAKE_LENGTH, snakePathLengths[i]);
    snakes[i].direction = (random(2) == 0) ? 1 : -1;
    snakes[i].active = true;
  }

  unsigned long startTime = millis();

  while (millis() - startTime < (unsigned long)duration_ms) {
    // Maintain at least 3-4 snakes active
    int activeCount = 0;
    for (int i = 0; i < NUM_SNAKES; i++) if (snakes[i].active) activeCount++;
    for (int i = 0; i < NUM_SNAKES && activeCount < 4; i++) {
      if (!snakes[i].active) {
        snakes[i].active = true;
        snakes[i].direction = (random(2) == 0) ? 1 : -1;
        snakes[i].headIndex = random(-SNAKE_LENGTH, snakePathLengths[i]);
        activeCount++;
      }
    }

    // Clear all LEDs completely
    for (int i = 0; i < NUM_LEDS; i++) {
      strip.setPixelColor(i, 0);
    }

    bool anyActive = false;

    // Draw all active snakes with fading tails
    for (int s = 0; s < NUM_SNAKES; s++) {
      if (snakes[s].active) {
        anyActive = true;
        int head = snakes[s].headIndex;
        for (int offset = 0; offset < SNAKE_LENGTH; offset++) {
          int pos = head - offset * snakes[s].direction;
          if (pos >= 0 && pos < snakePathLengths[s]) {
            uint8_t brightness = MIN_BRIGHTNESS + FADE_STEP * (SNAKE_LENGTH - 1 - offset);
            strip.setPixelColor(snakePaths[s][pos], colorWithBrightness(
              baseColors[s][0], baseColors[s][1], baseColors[s][2], brightness));
          }
        }

        snakes[s].headIndex += snakes[s].direction;

        // Deactivate if completely off path with buffer
        if (snakes[s].direction == 1) {
          if (snakes[s].headIndex >= snakePathLengths[s] + SNAKE_LENGTH)
            snakes[s].active = false;
        } else {
          if (snakes[s].headIndex < -SNAKE_LENGTH)
            snakes[s].active = false;
        }
      }
    }

    strip.show();
    delay(60); // Controls animation speed

    // If none active, restart all snakes randomly
    if (!anyActive) {
      for (int i = 0; i < NUM_SNAKES; i++) {
        snakes[i].active = true;
        snakes[i].direction = (random(2) == 0) ? 1 : -1;
        snakes[i].headIndex = random(-SNAKE_LENGTH, snakePathLengths[i]);
      }
    }
  }
}

int rollDiceUnique() {
  int roll;
  int attempts = 0;
  do {
    roll = random(1, 7);
    attempts++;
    if (attempts > 20) break;
  } while (rolled[roll - 1]);

  rolled[roll - 1] = true;

  bool allRolled = true;
  for (int i = 0; i < 6; i++) if (!rolled[i]) allRolled = false;
  if (allRolled) for (int i = 0; i < 6; i++) rolled[i] = false;

  return roll;
}

int* getSegment(int seg) {
  switch (seg) {
    case 0: return segA;
    case 1: return segB;
    case 2: return segC;
    case 3: return segD;
    case 4: return segE;
    case 5: return segF;
    case 6: return segG;
  }
  return nullptr;
}

int getSegmentSize(int seg) {
  switch (seg) {
    case 0: return sizeof(segA) / sizeof(int);
    case 1: return sizeof(segB) / sizeof(int);
    case 2: return sizeof(segC) / sizeof(int);
    case 3: return sizeof(segD) / sizeof(int);
    case 4: return sizeof(segE) / sizeof(int);
    case 5: return sizeof(segF) / sizeof(int);
    case 6: return sizeof(segG) / sizeof(int);
  }
  return 0;
}

void snakeSegmentCumulative(int seg[], int segSize, bool reverse = false) {
  if (reverse) {
    for (int i = segSize - 1; i >= 0; i--) {
      strip.setPixelColor(seg[i] - 1, NUMBER_COLOR);
      strip.show();
      delay(40);
    }
  }
  else {
    for (int i = 0; i < segSize; i++) {
      strip.setPixelColor(seg[i] - 1, NUMBER_COLOR);
      strip.show();
      delay(40);
    }
  }
}

void displayDigitSnake(int num) {
  for (int i = 0; i < NUM_LEDS; i++) strip.setPixelColor(i, 0);

  for (int orderIdx = 0; orderIdx < 7; orderIdx++) {
    int segNum = segmentOrder[num][orderIdx];
    if (segNum == -1) break;

    int* seg = getSegment(segNum);
    int size = getSegmentSize(segNum);
    bool rev = (segmentReverse[num][orderIdx] == 1);
    snakeSegmentCumulative(seg, size, rev);
  }
  strip.show();
}

void setup() {
  strip.begin();
  strip.setBrightness(255);
  strip.show();
  Serial.begin(115200);
  randomSeed(analogRead(0));
  Serial.println("Press 'R' to roll the dice!");
}

void loop() {
  if (Serial.available() > 0) {
    char input = Serial.read();
    if (input == 'r' || input == 'R') {
      // Clear LEDs
      for (int i = 0; i < NUM_LEDS; i++) strip.setPixelColor(i, 0);
      strip.show();

      // Run snakes animation for 3 seconds
      snakesAnimation(3000);

      // Roll dice and display number
      int diceRoll = rollDiceUnique();
      displayDigitSnake(diceRoll);
      Serial.print("Rolled: ");
      Serial.println(diceRoll);
    }
  }
}
