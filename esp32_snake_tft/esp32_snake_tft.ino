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
int dir = 1; // 0=Up, 1=Right, 2=Down, 3=Left
int foodX, foodY;
bool gameOver = false;
int score = 0;

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
  // Draw the new food
  tft.fillRect(foodX * BLOCK_SIZE, foodY * BLOCK_SIZE, BLOCK_SIZE, BLOCK_SIZE, ILI9341_RED);
}

void resetGame() {
  tft.fillScreen(ILI9341_BLACK);
  
  snakeLen = 5;
  dir = 1; // Start moving right
  score = 0;
  gameOver = false;
  
  // Center snake
  for (int i = 0; i < snakeLen; i++) {
    snakeX[i] = (GRID_W / 2) - i;
    snakeY[i] = GRID_H / 2;
    // Draw initial snake
    tft.fillRect(snakeX[i] * BLOCK_SIZE, snakeY[i] * BLOCK_SIZE, BLOCK_SIZE, BLOCK_SIZE, ILI9341_GREEN);
  }
  
  spawnFood();
}

void setup() {
  // Initialize buttons with internal pull-ups
  pinMode(BTN_UP, INPUT_PULLUP);
  pinMode(BTN_DOWN, INPUT_PULLUP);
  pinMode(BTN_LEFT, INPUT_PULLUP);
  pinMode(BTN_RIGHT, INPUT_PULLUP);
  
  // Initialize TFT
  tft.begin();
  tft.setRotation(1); // Landscape mode (320x240)
  tft.fillScreen(ILI9341_BLACK);
  
  // Start Screen
  tft.setTextColor(ILI9341_WHITE);
  tft.setTextSize(3);
  tft.setCursor(80, 100);
  tft.print("SNAKE ARCADE");
  delay(2000);
  
  // Floating pin 36 used for absolute random seed
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
    
    // Press ANY button to restart
    if (digitalRead(BTN_UP) == LOW || digitalRead(BTN_DOWN) == LOW || 
        digitalRead(BTN_LEFT) == LOW || digitalRead(BTN_RIGHT) == LOW) {
      delay(300); // Debounce
      resetGame();
    }
    return;
  }

  // --- Input Handling ---
  // Using LOW because buttons are tied to GND with internal pull-ups
  if (digitalRead(BTN_LEFT) == LOW && dir != 1) dir = 3;
  else if (digitalRead(BTN_RIGHT) == LOW && dir != 3) dir = 1;
  else if (digitalRead(BTN_UP) == LOW && dir != 2) dir = 0;
  else if (digitalRead(BTN_DOWN) == LOW && dir != 0) dir = 2;

  // Remember the tail position so we can erase it cleanly
  int tailX = snakeX[snakeLen - 1];
  int tailY = snakeY[snakeLen - 1];

  // Shift body array
  for (int i = snakeLen - 1; i > 0; i--) {
    snakeX[i] = snakeX[i - 1];
    snakeY[i] = snakeY[i - 1];
  }

  // Move head
  if (dir == 0) snakeY[0]--;       // Up
  else if (dir == 1) snakeX[0]++;  // Right
  else if (dir == 2) snakeY[0]++;  // Down
  else if (dir == 3) snakeX[0]--;  // Left

  // --- Collision Detection ---
  // 1. Wall Collision
  if (snakeX[0] < 0 || snakeX[0] >= GRID_W || snakeY[0] < 0 || snakeY[0] >= GRID_H) {
    gameOver = true;
    return;
  }
  // 2. Self Collision
  for (int i = 1; i < snakeLen; i++) {
    if (snakeX[0] == snakeX[i] && snakeY[0] == snakeY[i]) {
      gameOver = true;
      return;
    }
  }

  // --- Food Logic ---
  bool ateFood = false;
  if (snakeX[0] == foodX && snakeY[0] == foodY) {
    score++;
    if (snakeLen < 300) snakeLen++;
    ateFood = true;
    spawnFood();
  }

  // --- Rendering ---
  // Erase the old tail (only if we didn't just eat food)
  if (!ateFood) {
    tft.fillRect(tailX * BLOCK_SIZE, tailY * BLOCK_SIZE, BLOCK_SIZE, BLOCK_SIZE, ILI9341_BLACK);
  }
  
  // Draw the new head
  tft.fillRect(snakeX[0] * BLOCK_SIZE, snakeY[0] * BLOCK_SIZE, BLOCK_SIZE, BLOCK_SIZE, ILI9341_GREEN);

  // Speed of the game (lower number = faster game)
  delay(120); 
}
