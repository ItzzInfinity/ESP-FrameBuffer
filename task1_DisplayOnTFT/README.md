# Task 1: Display on TFT LCD

## Objective
Initialize and display content on a 1.8" TFT LCD display (128x160 pixels) with ST7735s controller using ESP32 or ESP32-C3.

## What This Code Does
- ✓ Initializes SPI communication with the TFT display
- ✓ Auto-detects ESP32 vs ESP32-C3 using #ifdef preprocessor directives
- ✓ Displays welcome message and system information
- ✓ Draws basic shapes (rectangles, circles, lines) to verify display functionality
- ✓ Provides serial feedback for debugging

## Hardware Setup

### Required Components
- ESP32 or ESP32-C3 microcontroller
- 1.8" TFT LCD display (128x160 pixels) with ST7735s controller
- USB cable for programming and power

### Wiring Diagram

| TFT Pin | Function      | ESP32 GPIO | ESP32-C3 GPIO |
|---------|---------------|-----------|---------------|
| VCC     | Power (3.3V)  | 3V3       | 3V3           |
| GND     | Ground        | GND       | GND           |
| CS      | Chip Select   | GPIO 5    | GPIO 7        |
| RESET   | Reset         | GPIO 4    | GPIO 8        |
| A0/DC   | Data/Command  | GPIO 2    | GPIO 10       |
| SDA     | MOSI (Data)   | GPIO 23   | GPIO 6        |
| SCK     | Clock         | GPIO 18   | GPIO 4        |
| LED     | Backlight     | 3V3       | 3V3           |

**Note:** The LED pin can be connected to 3V3 for always-on, or to a GPIO pin if brightness control is needed.

## Library Installation

1. Open Arduino IDE
2. Go to **Sketch** → **Include Library** → **Manage Libraries**
3. Search for **"Adafruit ST7735"**
4. Click **Install** (includes Adafruit GFX dependency)
5. Search for **"Adafruit GFX"** and install if not auto-installed

## How to Use

1. **Configure your board:**
   - Tools → Board → ESP32 (or select your specific board)
   - Tools → Port → Select your USB port
   - Tools → Upload Speed → 921600 baud

2. **Upload the code:**
   - Open `task1_DisplayOnTFT.ino` in Arduino IDE
   - Click **Upload** button

3. **Monitor output:**
   - Tools → Serial Monitor
   - Set baud rate to **115200**
   - You should see startup messages confirming initialization

4. **Expected result:**
   - TFT display shows colorful welcome screen
   - Startup messages appear in Serial Monitor
   - Display should remain active (no crashes)

## Code Structure

```
task1_DisplayOnTFT.ino
├── PIN DEFINITIONS (board-specific #ifdef)
├── GLOBAL OBJECTS (TFT display instance)
├── setup() - Hardware initialization
├── loop() - Main program loop
└── DISPLAY FUNCTIONS
    ├── displayStartupScreen() - Welcome message
    └── drawBackgroundPattern() - Test graphics
```

## Troubleshooting

| Issue | Solution |
|-------|----------|
| Display shows nothing | Check wiring, verify CS/DC pins match board definition |
| Serial monitor shows errors | Verify Adafruit libraries are installed correctly |
| Board not recognized | Install ESP32 board support: Tools → Board Manager → Search "ESP32" |
| Upload fails | Check USB cable, try different upload speed (slower first) |
| Display shows garbage | Try using `INITR_144GREENTAB` or `INITR_BLACKTAB` instead |

## Next Steps

Once this task works:
- **Task 2:** Add internet time/date display
- **Task 3:** Integrate DHT11 temperature/humidity sensor
- **Task 4:** Add SD card CSV logging functionality
- **Task 5:** Build simple webserver for CSV download

## Notes

- Code is modular with separate functions for different display operations
- Uses preprocessor directives for board compatibility
- Serial debugging enabled for development
- Pin definitions easily customizable in the header section
