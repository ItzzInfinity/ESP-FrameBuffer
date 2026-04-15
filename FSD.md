<!-- Functional Specification Document (FSD) -->

# espFrameBuffer

## Idea 
    I want to generate a framebuffer for the ESP32 or ESP32C3 microcontroller.
## Hardware
    ESP32 and ESP32C3 microcontroller.
    1.8 inch TFT LCD display with a resolution of 128x160 pixels ST7735s controller.
    SD card
    DHT11 temperature and humidity sensor
## Software
    Willing to use the Arduino IDE
## Tasks to get it working step by step
-   Task 1: WIth ESP Just Display anything on the TFT LCD display
-   Task 2: Display Time and date from internet on the TFT LCD display
    -   Task 2.1: Connect to WiFi and display connection status on TFT LCD display and all available WiFi networks - no need to refresh the whole page each time, just update the relevant section with the new information
-   Task 3: Display the temperature and humidity data from the DHT11 sensor on the TFT LCD display
    -   Task 3.1: Update the temperature and humidity data every 2 seconds (DHT11 minimum) without refreshing the whole page, just update the relevant section with the new information and fetch date and time from the internet and display it on the TFT LCD display. check previous temp and hum values and only update the display if there is a change to minimize unnecessary updates and reduce flickering. and clock will not have second hand, so update it every minute.
-   Task 4: Save the temperature and humidity data to the SD card in CSV format with a timestamp. The CSV file should have the following format: "timestamp,temperature,humidity". The timestamp should ISO 8601 format (e.g., "2026-04-17T14:32:45Z"). So it can be easily parsed by other applications.
    -   **Details**
        -   1. Display and SD card will share the same bus.
        -   2. Filename: "data_log.csv".
        -   3. Display will be in landscape orientation.
        -   4. Data logging interval: in top of the code.
        -   5. Handle SD card errors gracefully (e.g., no card, write failure) and display error messages on the TFT LCD display.
        -   6. Display Temperature, Humidity, and feels like temperature on the TFT LCD display. - refresh only the relevant section when the data changes.
-   Task 5: Simple Webserver to download the CSV file from the SD card.
## Guidelines
-   The code should be modular and well-documented.
-   at the top of the code, there should be a comment block that describes the purpose of the code, the hardware used, and any libraries required.
-   The code should be organized into functions for better readability and maintainability.
-   Use ifdef to change between ESP32 and ESP32C3 microcontroller.
-   for each task create folder and inside each folder create the corresponding .ino file (e.g., task1_codeName.ino, task2_codeName.ino, etc.)
## Rules
### 1. Think Before Coding

**Don't assume. Don't hide confusion. Surface tradeoffs.**

Before implementing:
- State your assumptions explicitly. If uncertain, ask.
- If multiple interpretations exist, present them - don't pick silently.
- If a simpler approach exists, say so. Push back when warranted.
- If something is unclear, stop. Name what's confusing. Ask.

### 2. Simplicity First

**Minimum code that solves the problem. Nothing speculative.**

- No features beyond what was asked.
- No abstractions for single-use code.
- No "flexibility" or "configurability" that wasn't requested.
- No error handling for impossible scenarios.
- If you write 200 lines and it could be 50, rewrite it.

Ask yourself: "Would a senior engineer say this is overcomplicated?" If yes, simplify.

### 3. Surgical Changes

**Touch only what you must. Clean up only your own mess.**

When editing existing code:
- Don't "improve" adjacent code, comments, or formatting.
- Don't refactor things that aren't broken.
- Match existing style, even if you'd do it differently.
- If you notice unrelated dead code, mention it - don't delete it.

When your changes create orphans:
- Remove imports/variables/functions that YOUR changes made unused.
- Don't remove pre-existing dead code unless asked.

The test: Every changed line should trace directly to the user's request.

### 4. Goal-Driven Execution

**Define success criteria. Loop until verified.**

Transform tasks into verifiable goals:
- "Add validation" → "Write tests for invalid inputs, then make them pass"
- "Fix the bug" → "Write a test that reproduces it, then make it pass"
- "Refactor X" → "Ensure tests pass before and after"

For multi-step tasks, state a brief plan:
```
1. [Step] → verify: [check]
2. [Step] → verify: [check]
3. [Step] → verify: [check]
```

Strong success criteria let you loop independently. Weak criteria ("make it work") require constant clarification.

---
## Pinout
### For the 1.8 inch TFT LCD display with a resolution of 128x160 pixels ST7735s controller, the pinout is as follows:
| Pin Name | Name |
|----------|----------|
| VCC      | 3.3V     |
| GND      | Ground   |
| CS       | Chip Select |
| RESET    | Reset (Should be pulled high)   |
| A0       | Data/Command |
| SDA      | MOSI |
| SCK      | SCK |
| LED      | Backlight |
### For the ESP32-C3 microcontroller, the pinout is as follows:
| Board Pin | ESP32-C3 GPIO | Primary Function | Secondary Functions|
|-----------|-----------|-----------|-----------|
| 5V |— |5V Power Input |USB-C Power Rail|
| GND |— |Ground |Common Ground|
| 3V3 |— |3.3V Output/Input |LDO Regulator Output|
| IO0 |GPIO 0 |Digital I/O |"ADC1_CH0 | RTC"|
| IO1 |GPIO 1 |Digital I/O |"ADC1_CH1 | RTC"|
| IO2 |GPIO 2 |Digital I/O |"ADC1_CH2 | RTC | Strapping"|
| IO3 |GPIO 3 |Digital I/O |"ADC1_CH3 | RTC"|
| IO4 |GPIO 4 |Digital I/O |"ADC1_CH4 | RTC | SPI_SCK"|
| IO5 |GPIO 5 |Digital I/O |"ADC1_CH5 | RTC | SPI_MISO"|
| IO6 |GPIO 6 |Digital I/O |SPI_MOSI|
| IO7 |GPIO 7 |Digital I/O |SPI_CS|
| IO8 |GPIO 8 |Built-in LED |"I2C_SDA | Strapping (Active Low)"|
| IO9 |GPIO 9 |BOOT Button |"I2C_SCL | Strapping"|
| IO10 |GPIO 10 |Digital I/O |FSPICS0|
| IO20 |GPIO 20 |UART RX |Digital I/O|
| IO21 |GPIO 21 |UART TX |Digital I/O|
### For ESP32 38 pin microcontroller, the pinout is as follows:

| Pin |Label |Function |Description |
|-----------|-----------|-----------|-----------|
| 1 |3V3 |Power |3.3V Output/Input |
| 2 |EN |Reset |"High = On | Low = Reset" |
| 3 |VP |GPIO 36 |"ADC1_CH0 | Input Only" |
| 4 |VN |GPIO 39 |"ADC1_CH3 | Input Only" |
| 5 |D34 |GPIO 34 |"ADC1_CH6 | Input Only" |
| 6 |D35 |GPIO 35 |"ADC1_CH7 | Input Only" |
| 7 |D32 |GPIO 32 |"ADC1_CH4 | Touch 9 | Crystal" |
| 8 |D33 |GPIO 33 |"ADC1_CH5 | Touch 8 | Crystal" |
| 9 |D25 |GPIO 25 |"ADC2_CH8 | DAC 1" |
| 10 |D26 |GPIO 26 |"ADC2_CH9 | DAC 2" |
| 11 |D27 |GPIO 27 |"ADC2_CH7 | Touch 7" |
| 12 |D14 |GPIO 14 |"ADC2_CH6 | Touch 6 | HSPI_CLK" |
| 13 |D12 |GPIO 12 |"ADC2_CH5 | Touch 5 | HSPI_MISO" |
| 14 |D13 |GPIO 13 |"ADC2_CH4 | Touch 4 | HSPI_MOSI" |
| 15 |GND |Ground |Common Ground |
| 16 |D23 |GPIO 23 |VSPI_MOSI |
| 17 |D22 |GPIO 22 |I2C_SCL |
| 18 |TX0 |GPIO 1 |UART TX |
| 19 |RX0 |GPIO 3 |UART RX |
| 20 |D21 |GPIO 21 |I2C_SDA |
| 21 |D19 |GPIO 19 |VSPI_MISO |
| 22 |D18 |GPIO 18 |VSPI_CLK |
| 23 |D5 |GPIO 5 |VSPI_CS |
| 24 |D17 |GPIO 17 |UART2 TX |
| 25 |D16 |GPIO 16 |UART2 RX |
| 26 |D4 |GPIO 4 |"ADC2_CH0, Touch 0" |
| 27 |D0 |GPIO 0 |Boot Mode (Pull low to flash) |
| 28 |D2 |GPIO 2 |"ADC2_CH2, Touch 2, Internal LED" |
| 29 |D15 |GPIO 15 |"ADC2_CH3, Touch 3, HSPI_CS" |
| 30 |D8 |GPIO 8 |SDIO Data 1 (Internal Flash - Avoid) |
| 31 |D7 |GPIO 7 |SDIO Data 0 (Internal Flash - Avoid) |
| 32 |D6 |GPIO 6 |SDIO CLK (Internal Flash - Avoid) |
| 33 |D11 |GPIO 11 |SDIO CMD (Internal Flash - Avoid) |
| 34 |D10 |GPIO 10 |SDIO Data 3 (Internal Flash - Avoid) |
| 35 |D9 |GPIO 9 |SDIO Data 2 (Internal Flash - Avoid) |
| 36 |D1 |GPIO 1 |UART TX (Duplicate) |
| 37 |GND |Ground |Common Ground |
| 38 |5V |Power |5V Input (VBUS) |
| 4 |VN |GPIO 39 |"ADC1_CH3 | Input Only" |
| 5 |D34 |GPIO 34 |"ADC1_CH6 | Input Only" |
| 6 |D35 |GPIO 35 |"ADC1_CH7 | Input Only" |
| 7 |D32 |GPIO 32 |"ADC1_CH4 | Touch 9 | Crystal" |
| 8 |D33 |GPIO 33 |"ADC1_CH5 | Touch 8 | Crystal" |
| 9 |D25 |GPIO 25 |"ADC2_CH8 | DAC 1" |
| 10 |D26 |GPIO 26 |"ADC2_CH9 | DAC 2" |
| 11 |D27 |GPIO 27 |"ADC2_CH7 | Touch 7" |
| 12 |D14 |GPIO 14 |"ADC2_CH6 | Touch 6 | HSPI_CLK" |
| 13 |D12 |GPIO 12 |"ADC2_CH5 | Touch 5 | HSPI_MISO" |
| 14 |D13 |GPIO 13 |"ADC2_CH4 | Touch 4 | HSPI_MOSI" |
| 15 |GND |Ground |Common Ground |
| 16 |D23 |GPIO 23 |VSPI_MOSI |
| 17 |D22 |GPIO 22 |I2C_SCL |
| 18 |TX0 |GPIO 1 |UART TX |
| 19 |RX0 |GPIO 3 |UART RX |
| 20 |D21 |GPIO 21 |I2C_SDA |
| 21 |D19 |GPIO 19 |VSPI_MISO |
| 22 |D18 |GPIO 18 |VSPI_CLK |
| 23 |D5 |GPIO 5 |VSPI_CS |
| 24 |D17 |GPIO 17 |UART2 TX |
| 25 |D16 |GPIO 16 |UART2 RX |
| 26 |D4 |GPIO 4 |"ADC2_CH0, Touch 0" |
| 27 |D0 |GPIO 0 |Boot Mode (Pull low to flash) |
| 28 |D2 |GPIO 2 |"ADC2_CH2, Touch 2, Internal LED" |
| 29 |D15 |GPIO 15 |"ADC2_CH3, Touch 3, HSPI_CS" |
| 30 |D8 |GPIO 8 |SDIO Data 1 (Internal Flash - Avoid) |
| 31 |D7 |GPIO 7 |SDIO Data 0 (Internal Flash - Avoid) |
| 32 |D6 |GPIO 6 |SDIO CLK (Internal Flash - Avoid) |
| 33 |D11 |GPIO 11 |SDIO CMD (Internal Flash - Avoid) |
| 34 |D10 |GPIO 10 |SDIO Data 3 (Internal Flash - Avoid) |
| 35 |D9 |GPIO 9 |SDIO Data 2 (Internal Flash - Avoid) |
| 36 |D1 |GPIO 1 |UART TX (Duplicate) |
| 37 |GND |Ground |Common Ground |
| 38 |5V |Power |5V Input (VBUS) |
| 37 |GND |Ground |Common Ground |
| 38 |5V |Power |5V Input (VBUS) |