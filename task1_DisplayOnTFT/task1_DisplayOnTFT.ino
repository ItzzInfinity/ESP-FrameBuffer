/*
 * ============================================================================
 * PROJECT: espFrameBuffer - Task 1: Display on TFT LCD
 * ============================================================================
 * PURPOSE:
 *   Initialize and display content on a 1.8" TFT LCD display (128x160 pixels)
 *   with ST7735s controller. This is the foundation task for framebuffer output.
 *
 * HARDWARE:
 *   - Microcontroller: ESP32 or ESP32-C3
 *   - Display: 1.8" TFT LCD (128x160 pixels) with ST7735s controller
 *   - Communication: SPI (4-wire)
 *
 * REQUIRED LIBRARIES:
 *   - Adafruit_ST7735 (https://github.com/adafruit/Adafruit-ST7735-Library)
 *   - Adafruit_GFX (https://github.com/adafruit/Adafruit-GFX-Library)
 *
 * INSTALLATION:
 *   Arduino IDE → Sketch → Include Library → Manage Libraries
 *   Search for "Adafruit ST7735" and "Adafruit GFX" → Install
 *
 * WIRING:
 *   TFT Pin  | Function      | ESP32/C3 Pin
 *   ---------|---------------|------------------
 *   VCC      | Power (3.3V)  | 3V3
 *   GND      | Ground        | GND
 *   CS       | Chip Select   | GPIO 7 (ESP32-C3) / GPIO 5 (ESP32)
 *   RESET    | Reset         | GPIO 8 (ESP32-C3) / GPIO 4 (ESP32)
 *   A0/DC    | Data/Command  | GPIO 10 (ESP32-C3) / GPIO 2 (ESP32)
 *   SDA/MOSI | Data In       | GPIO 6 (ESP32-C3) / GPIO 23 (ESP32)
 *   SCK      | Clock         | GPIO 4 (ESP32-C3) / GPIO 18 (ESP32)
 *   LED      | Backlight     | 3V3 (or PWM GPIO for brightness)
 *
 * ============================================================================
 */

#include <Adafruit_ST7735.h>
#include <Adafruit_GFX.h>
#include <SPI.h>

// ============================================================================
// PIN DEFINITIONS (Board-specific)
// ============================================================================

#if defined(ESP32) && !defined(CONFIG_IDF_TARGET_ESP32C3)
  // ESP32 (38-pin variant) SPI pin definitions
  #define TFT_CS   5    // Chip Select
  #define TFT_RST  4    // Reset
  #define TFT_DC   2    // Data/Command
  #define TFT_MOSI 23   // SDA/MOSI
  #define TFT_SCK  18   // Clock
  #define SPI_FREQ 40000000  // 40 MHz

#elif defined(CONFIG_IDF_TARGET_ESP32C3) || defined(ESP32C3)
  // ESP32-C3 SPI pin definitions
  #define TFT_CS   7    // Chip Select
  #define TFT_RST  8    // Reset
  #define TFT_DC   10   // Data/Command
  #define TFT_MOSI 6    // SDA/MOSI
  #define TFT_SCK  4    // Clock
  #define SPI_FREQ 40000000  // 40 MHz

#else
  #error "Unsupported microcontroller. Please use ESP32 or ESP32-C3."
#endif

// ============================================================================
// GLOBAL OBJECTS
// ============================================================================

// Create TFT display object (using software SPI)
Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_MOSI, TFT_SCK, TFT_RST);

// ============================================================================
// SETUP - Initialize hardware and display
// ============================================================================

void setup() {
  Serial.begin(115200);
  delay(2000);
  
  // Log startup information
  Serial.println("\n\n===========================================");
  Serial.println("espFrameBuffer - Task 1: Display on TFT LCD");
  Serial.println("===========================================");
  
  #if defined(ESP32) && !defined(CONFIG_IDF_TARGET_ESP32C3)
    Serial.println("Board: ESP32");
  #elif defined(CONFIG_IDF_TARGET_ESP32C3) || defined(ESP32C3)
    Serial.println("Board: ESP32-C3");
  #endif
  
  Serial.println("Initializing TFT display...");
  
  // Initialize display with Adafruit_ST7735
  // Parameters: (width=128, height=160, rotation)
  tft.initR(INITR_18GREENTAB);  // Initialize with default 18-bit color ST7735S
  
  // Set rotation (0=Portrait, 1=Landscape, 2=Portrait-flipped, 3=Landscape-flipped)
  tft.setRotation(2);
  
  // Fill screen with black background
  tft.fillScreen(ST7735_BLACK);
  
  Serial.println("✓ Display initialized successfully");
  Serial.println("===========================================\n");
  
  // Draw initial content
  displayStartupScreen();
}

// ============================================================================
// MAIN LOOP
// ============================================================================

void loop() {
  // For now, hold the startup screen, loop can be extended for future tasks
  delay(1000);
}

// ============================================================================
// DISPLAY FUNCTIONS
// ============================================================================

/*
 * displayStartupScreen()
 * Displays welcome message and system information on the TFT LCD
 */
void displayStartupScreen() {
  // Clear screen
  tft.fillScreen(ST7735_BLACK);
  
  // Draw title
  tft.setCursor(10, 10);
  tft.setTextColor(ST7735_CYAN);
  tft.setTextSize(2);
  tft.println("espFrame");
  tft.println("Buffer");
  
  // Draw separator line
  tft.drawLine(0, 50, 128, 50, ST7735_CYAN);
  
  // Draw task information
  tft.setCursor(5, 60);
  tft.setTextColor(ST7735_WHITE);
  tft.setTextSize(1);
  tft.println("Task 1: Display");
  tft.println("Status: ACTIVE");
  
  // Draw some decorative shapes
  drawBackgroundPattern();
  
  // Draw footer with display info
  tft.setCursor(5, 140);
  tft.setTextColor(ST7735_MAGENTA);
  tft.setTextSize(1);
  tft.println("128x160 ST7735S");
}

/*
 * drawBackgroundPattern()
 * Draws decorative shapes on the display
 */
void drawBackgroundPattern() {
  // Draw colored rectangles in corners
  tft.drawRect(5, 100, 20, 20, ST7735_RED);
  tft.drawRect(103, 100, 20, 20, ST7735_YELLOW);
  
  // Draw small circles
  tft.drawCircle(64, 110, 10, ST7735_GREEN);
  
  // Draw a simple line pattern
  for (int i = 0; i < 10; i++) {
    tft.drawLine(0, 120 + i, 128, 120 + i, ST7735_BLUE);
    delay(10);  // Optional: create drawing animation effect
  }
}

// ============================================================================
// END OF FILE
// ============================================================================
