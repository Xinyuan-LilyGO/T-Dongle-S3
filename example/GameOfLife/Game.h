#include "Arduino.h"
#include "pin_config.h"
#include "TFT_eSPI.h"

TFT_eSPI tft = TFT_eSPI();

const int WIDTH = 180;
const int HEIGHT = 80;
const int CELL_SIZE = 2;
int grid[WIDTH / CELL_SIZE][HEIGHT / CELL_SIZE];
int nextGrid[WIDTH / CELL_SIZE][HEIGHT / CELL_SIZE];

void setup() {
  Serial.begin(115200);
  pinMode(TFT_LEDA_PIN, OUTPUT);
  
  tft.init();
  tft.setRotation(1);
  tft.fillScreen(TFT_BLACK);
  digitalWrite(TFT_LEDA_PIN, 0);

  // Set initial state
  for (int i = 0; i < WIDTH / CELL_SIZE; i++) {
    for (int j = 0; j < HEIGHT / CELL_SIZE; j++) {
      grid[i][j] = (random(100) > 70) ? 1 : 0;
    }
  }
}

void loop() {
  drawGrid();
  computeNextGrid();
  delay(100);
}

void drawGrid() {
  for (int i = 0; i < WIDTH / CELL_SIZE; i++) {
    for (int j = 0; j < HEIGHT / CELL_SIZE; j++) {
      if (grid[i][j] == 1) {
        tft.fillRect(i * CELL_SIZE, j * CELL_SIZE, CELL_SIZE, CELL_SIZE, TFT_WHITE);
      } else {
        tft.fillRect(i * CELL_SIZE, j * CELL_SIZE, CELL_SIZE, CELL_SIZE, TFT_BLACK);
      }
    }
  }
}

void computeNextGrid() {
  for (int x = 1; x < WIDTH / CELL_SIZE - 1; x++) {
    for (int y = 1; y < HEIGHT / CELL_SIZE - 1; y++) {
      int aliveNeighbors = 0;
      for (int i = -1; i <= 1; i++) {
        for (int j = -1; j <= 1; j++) {
          aliveNeighbors += grid[x + i][y + j];
        }
      }
      aliveNeighbors -= grid[x][y];

      if (grid[x][y] == 1 && aliveNeighbors < 2) {
        nextGrid[x][y] = 0;
      } else if (grid[x][y] == 1 && aliveNeighbors > 3) {
        nextGrid[x][y] = 0;
      } else if (grid[x][y] == 0 && aliveNeighbors == 3) {
        nextGrid[x][y] = 1;
      } else {
        nextGrid[x][y] = grid[x][y];
      }
    }
  }

  for (int i = 0; i < WIDTH / CELL_SIZE; i++) {
    for (int j = 0; j < HEIGHT / CELL_SIZE; j++) {
      grid[i][j] = nextGrid[i][j];
    }
  }
}
