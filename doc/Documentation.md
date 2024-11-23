
# Project Documentation

## 1. Project Overview

- Purpose: Enhance the Euro Truck Simulator 2 experience with real-time telemetry integration, offering immersive control through custom hardware.

- Core Features:
    - Easy Access to ETS2 Functions: Allows users to manage in-game functions efficiently.
    - Light Feedback: Uses light indicators to signal important events directly from the game, improving situational awareness.

## 2. Project Structure

The structure is organized to keep code modular, maintainable, and easy to navigate. Here’s an overview of each file:

- **`main.c`**: Main function used to create tasks and Semaphores.

- **`ets2ctrl.c`**: Control Leds doing blinking and power on or off.

- **`httpHandler.c`**: Connecting the http server and decoding JSON data.

- **`spiHandler.c`**: Interface of TLC59116 using i2c protocol.

- **`tasksHandler.c`**: Handle the http requests and using data form the decoding JSON to LEDs.

- **`wifiHandler.c`**: Handle the wifi connection.

## 3. Hardware



Connect the switch on the
EG STARTS BOARD:
S1 - K1
S2 - K2
S3 - K3
S4 - K4
S5 - L1
S6 - R1
S7 - L2
S8 - R2
S9 - SE
S10 - ST
S11 - K11
S12 - K12
S13 - AL
S14 - AR
S15 - AD
