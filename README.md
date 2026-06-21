# 🐍 Retro Arcade Snake (ESP32 + TFT)

A classic arcade-style Snake game built from scratch using an **ESP32** microcontroller, a vibrant **ILI9341 320x240 TFT display**, and four discrete push buttons for that authentic retro feel. 

This project demonstrates how to use hardware SPI for fast screen updates and implements a smart rendering technique to prevent screen flickering during gameplay.

## ✨ Features
* **Smooth Rendering:** Only updates the moving parts of the snake (head and tail) rather than clearing the entire screen, completely eliminating TFT flicker.
* **Hardware SPI:** Utilizes the ESP32's high-speed VSPI pins (including MISO) for instantaneous graphics processing and future expansion.
* **Classic Mechanics:** Includes wall collision, self-collision, score tracking, and an escalating snake length.
* **True Randomization:** Uses an unconnected analog pin to generate a true random seed, ensuring food spawns in a different pattern every game.
* **Internal Pull-ups:** Simplifies wiring by utilizing the ESP32's internal pull-up resistors for the directional buttons.

---

## 🛠️ Hardware Requirements
* **1x** ESP32 Development Board (e.g., DevKit V1)
* **1x** ILI9341 TFT LCD Display (SPI)
* **4x** Tactile Push Buttons
* **1x** 330Ω Resistor (for backlight protection)
* Breadboard & Jumper Wires

---

## 🔌 Wiring Guide

### TFT Display (Hardware SPI)
| TFT Pin | ESP32 Pin | Description |
| :--- | :--- | :--- |
| **VCC** | `3V3` | Power |
| **GND** | `GND` | Ground |
| **CS** | `GPIO 15` | Chip Select |
| **RESET** | `GPIO 4` | Reset |
| **DC** | `GPIO 2` | Data/Command |
| **MOSI** | `GPIO 23` | SPI Data (Send) |
| **SCK** | `GPIO 18` | SPI Clock |
| **LED** | `3V3` | Backlight (Connect via **330Ω Resistor**) |
| **MISO** | `GPIO 19` | SPI Data (Receive/SD Card) |

### Directional Controls
*Wire one leg of each button to the ESP32 pin, and the other leg directly to `GND`.*

| Button | ESP32 Pin |
| :--- | :--- |
| **UP** | `GPIO 32` |
| **DOWN** | `GPIO 33` |
| **LEFT** | `GPIO 25` |
| **RIGHT** | `GPIO 26` |

---

## 💻 Software & Libraries
To run this code, you will need the Arduino IDE installed with the ESP32 board manager. 

Install the following libraries via the **Arduino Library Manager**:
1. `Adafruit GFX Library` by Adafruit
2. `Adafruit ILI9341` by Adafruit

---

## 🚀 How to Run
1. Clone this repository to your local machine.
2. Open the `.ino` file in the Arduino IDE.
3. Select your ESP32 board and COM port from the **Tools** menu.
4. Click **Upload**.
5. Once uploaded, the Start Screen will appear for 2 seconds before the game begins!

## 🎮 How to Play
* Use the **UP**, **DOWN**, **LEFT**, and **RIGHT** buttons to steer the snake.
* Eat the red apples to grow longer and increase your score.
* Do not hit the edges of the screen or bite your own tail!
* If you get a **Game Over**, press any of the four direction buttons to restart instantly.
