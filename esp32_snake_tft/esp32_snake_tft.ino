#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>

// TFT Display Pins (Standard ESP32 VSPI)
#define TFT_CS   15
#define TFT_DC   2
#define TFT_RST  4
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_RST);

// Push Button Pins
#define BTN_UP    32
#define BTN_DOWN  33
#define BTN_LEFT  25
#define BTN_RIGHT 26

// Game Settings
#define BLOCK_SIZE 10
#define GRID_W (320 / BLOCK_SIZE)
#define GRID_H (240 / BLOCK_SIZE)

// Game Variables
int snakeX[300];
int snakeY[300];
int snakeLen = 5;
int currentDir = 1; // Actual moving direction: 0=Up, 1=Right, 2=Down, 3=Left
int nextDir = 1;    // Queued direction from button press
int foodX, foodY;
bool gameOver = false;
int score = 0;

// Timing Variables (Replaces delay)
unsigned long lastMoveTime = 0;
int moveDelay = 70; // Lower is faster

void spawnFood() {
  bool valid = false;
  while (!valid) {
    foodX = random(0, GRID_W);
    foodY = random(0, GRID_H);
    valid = true;
    for (int i = 0; i < snakeLen; i++) {
      if (snakeX[i] == foodX && snakeY[i] == foodY) {
        valid = false;
        break;
      }
    }
  }
  tft.fillRect(foodX * BLOCK_SIZE, foodY * BLOCK_SIZE, BLOCK_SIZE, BLOCK_SIZE, ILI9341_RED);
}

void resetGame() {
  tft.fillScreen(ILI9341_BLACK);
  
  snakeLen = 5;
  currentDir = 1;
  nextDir = 1;
  score = 0;
  gameOver = false;
  
  // Center snake
  for (int i = 0; i < snakeLen; i++) {
    snakeX[i] = (GRID_W / 2) - i;
    snakeY[i] = GRID_H / 2;
    tft.fillRect(snakeX[i] * BLOCK_SIZE, snakeY[i] * BLOCK_SIZE, BLOCK_SIZE, BLOCK_SIZE, ILI9341_GREEN);
  }
  
  spawnFood();
  lastMoveTime = millis();
}

void setup() {
  pinMode(BTN_UP, INPUT_PULLUP);
  pinMode(BTN_DOWN, INPUT_PULLUP);
  pinMode(BTN_LEFT, INPUT_PULLUP);
  pinMode(BTN_RIGHT, INPUT_PULLUP);
  
  tft.begin();
  tft.setRotation(1); 
  tft.fillScreen(ILI9341_BLACK);
  
  tft.setTextColor(ILI9341_WHITE);
  tft.setTextSize(3);
  tft.setCursor(80, 100);
  tft.print("SNAKE ARCADE");
  delay(2000);
  
  randomSeed(analogRead(36));
  resetGame();
}

void loop() {
  // --- Game Over State ---
  if (gameOver) {
    tft.setTextColor(ILI9341_RED);
    tft.setTextSize(4);
    tft.setCursor(50, 80);
    tft.print("GAME OVER");
    
    tft.setTextColor(ILI9341_WHITE);
    tft.setTextSize(2);
    tft.setCursor(100, 140);
    tft.print("Score: ");
    tft.print(score);
    
    if (digitalRead(BTN_UP) == LOW || digitalRead(BTN_DOWN) == LOW || 
        digitalRead(BTN_LEFT) == LOW || digitalRead(BTN_RIGHT) == LOW) {
      delay(300); // Debounce for restart only
      resetGame();
    }
    return;
  }

  // --- 1. INSTANT INPUT HANDLING ---
  // This runs continuously, so no button presses are ever missed.
  if (digitalRead(BTN_LEFT) == LOW && currentDir != 1) nextDir = 3;
  else if (digitalRead(BTN_RIGHT) == LOW && currentDir != 3) nextDir = 1;
  else if (digitalRead(BTN_UP) == LOW && currentDir != 2) nextDir = 0;
  else if (digitalRead(BTN_DOWN) == LOW && currentDir != 0) nextDir = 2;

  // --- 2. TIMED MOVEMENT ---
  // Only move the snake when enough time has passed
  if (millis() - lastMoveTime >= moveDelay) {
    lastMoveTime = millis();
    
    // Lock in the queued direction
    currentDir = nextDir;

    // Remember the exact tail position
    int tailX = snakeX[snakeLen - 1];
    int tailY = snakeY[snakeLen - 1];

    // Shift body array
    for (int i = snakeLen - 1; i > 0; i--) {
      snakeX[i] = snakeX[i - 1];
      snakeY[i] = snakeY[i - 1];
    }

    // Move head
    if (currentDir == 0) snakeY[0]--;
    else if (currentDir == 1) snakeX[0]++;
    else if (currentDir == 2) snakeY[0]++;
    else if (currentDir == 3) snakeX[0]--;

    // Wall Collision
    if (snakeX[0] < 0 || snakeX[0] >= GRID_W || snakeY[0] < 0 || snakeY[0] >= GRID_H) {
      gameOver = true;
      return;
    }
    
    // Self Collision
    for (int i = 1; i < snakeLen; i++) {
      if (snakeX[0] == snakeX[i] && snakeY[0] == snakeY[i]) {
        gameOver = true;
        return;
      }
    }

    // Food Logic
    bool ateFood = false;
    if (snakeX[0] == foodX && snakeY[0] == foodY) {
      score++;
      if (snakeLen < 300) {
        // FIX: Place the new tail piece exactly where the old tail was
        snakeX[snakeLen] = tailX;
        snakeY[snakeLen] = tailY;
        snakeLen++;
      }
      ateFood = true;
      spawnFood();
    }

    // Rendering
    if (!ateFood) {
      tft.fillRect(tailX * BLOCK_SIZE, tailY * BLOCK_SIZE, BLOCK_SIZE, BLOCK_SIZE, ILI9341_BLACK);
    }
    tft.fillRect(snakeX[0] * BLOCK_SIZE, snakeY[0] * BLOCK_SIZE, BLOCK_SIZE, BLOCK_SIZE, ILI9341_GREEN);
  }
}
