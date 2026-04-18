/*
 * ============================================================================
 * PROJECT: espFrameBuffer - Task 5: Futuristic Climate Display
 * ============================================================================
 * PURPOSE:
 *   Modern, sleek climate monitoring display with dynamic colors and 
 *   smooth animations. Shows temperature, humidity, and feels-like in 
 *   a visually appealing interface. No WiFi, no SD card, display only.
 *
 * HARDWARE:
 *   - ESP32 or ESP32-C3
 *   - 1.8" TFT LCD (128x160 pixels) with ST7735s controller
 *   - DHT11 temperature and humidity sensor
 *   - Landscape orientation
 *
 * DESIGN PHILOSOPHY:
 *   - Modern, futuristic look
 *   - Large, easy-to-read values
 *   - Color-coded (blue=cold, green=comfortable, red=hot)
 *   - Partial screen updates (no flicker)
 *   - Minimal, clean interface
 *
 * REQUIRED LIBRARIES:
 *   - Adafruit_ST7735
 *   - Adafruit_GFX
 *   - Adafruit_DHT
 *
 * ============================================================================
 */

#include <Adafruit_ST7735.h>
#include <Adafruit_GFX.h>
#include <DHT.h>

// ============================================================================
// CONFIGURATION
// ============================================================================

#define DHT_PIN      14
#define DHT_TYPE     DHT11

DHT dht(DHT_PIN, DHT_TYPE);

// ============================================================================
// COLORS (RGB565) - Cyberpunk Dashboard Theme
// ============================================================================

#define COLOR_BLACK        0x0000   // Pure black background
#define COLOR_CARD_BG      0x0410   // Very dark blue-gray (card background)
#define COLOR_WHITE        0xFFFF
#define COLOR_DARK_GRAY    0x2104
#define COLOR_GRAY         0x4208
#define COLOR_LIGHT_GRAY   0xC618

// Temperature colors (dynamic)
#define COLOR_COLD         0x001F   // Deep blue (< 15°C)
#define COLOR_COOL         0x041F   // Blue (15-18°C)
#define COLOR_COMFORT      0x07E0   // Neon green (18-24°C)
#define COLOR_WARM         0xFE00   // Neon orange (24-30°C)
#define COLOR_HOT          0xF800   // Bright red (> 30°C)

// Accent colors
#define COLOR_CYAN         0x07FF   // Bright cyan (primary accent)
#define COLOR_ORANGE       0xFD20
#define COLOR_GRID         0x0410   // Subtle divider color
#define COLOR_ACCENT       0x07FF   // Cyan (all accents, replaces magenta)

// ============================================================================
// PIN DEFINITIONS (Board-specific)
// ============================================================================

#if defined(ESP32) && !defined(CONFIG_IDF_TARGET_ESP32C3)
  #define TFT_CS   5
  #define TFT_RST  4
  #define TFT_DC   2
  #define TFT_MOSI 23
  #define TFT_SCK  18
#elif defined(CONFIG_IDF_TARGET_ESP32C3) || defined(ESP32C3)
  #define TFT_CS   7
  #define TFT_RST  8
  #define TFT_DC   10
  #define TFT_MOSI 6
  #define TFT_SCK  4
#endif

// ============================================================================
// GLOBALS
// ============================================================================

Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_MOSI, TFT_SCK, TFT_RST);

float prev_temperature = -999;
float prev_humidity = -999;
float prev_feels_like = -999;

unsigned long last_dht_read = 0;

#define DHT_READ_INTERVAL 2000

// ============================================================================
// SETUP
// ============================================================================

void setup() {
  Serial.begin(115200);
  delay(2000);
  
  Serial.println("\n\nTask 5: Futuristic Climate Display");
  
  // Initialize display in landscape
  tft.initR(INITR_BLACKTAB);
  tft.setRotation(1);  // Landscape (160 wide × 128 tall)
  tft.fillScreen(COLOR_BLACK);
  
  Serial.println("✓ Display initialized");
  
  // Initialize DHT11
  dht.begin();
  Serial.println("✓ DHT11 initialized on GPIO " + String(DHT_PIN));
  
  // Draw initial UI
  drawInitialUI();
}

// ============================================================================
// MAIN LOOP
// ============================================================================

void loop() {
  unsigned long now = millis();
  
  // Read DHT11 every 2 seconds
  if (now - last_dht_read >= DHT_READ_INTERVAL) {
    last_dht_read = now;
    readAndUpdateDHT();
  }
  
  delay(100);
}

// ============================================================================
// DHT11 FUNCTIONS
// ============================================================================

void readAndUpdateDHT() {
  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();
  
  if (isnan(temperature) || isnan(humidity)) {
    Serial.println("✗ Failed to read DHT11");
    displaySensorError();
    return;
  }
  
  float feels_like = calculateFeelsLike(temperature, humidity);
  
  // Only update display if values changed significantly
  if (abs(temperature - prev_temperature) >= 0.5) {
    updateTemperatureDisplay(temperature);
    prev_temperature = temperature;
    Serial.printf("T: %.1f°C\n", temperature);
  }
  
  if (abs(humidity - prev_humidity) >= 2.0) {
    updateHumidityDisplay(humidity);
    prev_humidity = humidity;
    Serial.printf("H: %.0f%%\n", humidity);
  }
  
  if (abs(feels_like - prev_feels_like) >= 0.5) {
    updateFeelsLikeDisplay(feels_like);
    prev_feels_like = feels_like;
    Serial.printf("F: %.1f°C\n", feels_like);
  }
}

float calculateFeelsLike(float temp, float humidity) {
  // Use DHT11 library's built-in heat index calculation
  // false = not using Fahrenheit (using Celsius)
  return dht.computeHeatIndex(temp, humidity, false);
}

// ============================================================================
// COLOR FUNCTIONS
// ============================================================================

uint16_t getTempColor(float temp) {
  // Dynamic color based on temperature
  if (temp < 15) return COLOR_COLD;           // < 15°C - Deep blue
  if (temp < 18) return COLOR_COOL;           // 15-18°C - Blue
  if (temp < 24) return COLOR_COMFORT;        // 18-24°C - Green
  if (temp < 30) return COLOR_WARM;           // 24-30°C - Orange
  return COLOR_HOT;                           // > 30°C - Red
}

// ============================================================================
// ICON DRAWING FUNCTIONS
// ============================================================================

void drawThermometerIcon(int x, int y, uint16_t color) {
  // Frame
  tft.drawRect(x, y, 15, 15, color);
  
  // Bulb (top 2 rows of frame)
  tft.fillRect(x + 1, y + 1, 13, 3, color);
  
  // Stem (vertical line down center)
  tft.drawLine(x + 7, y + 4, x + 7, y + 12, color);
  tft.drawLine(x + 8, y + 4, x + 8, y + 12, color);
  
  // Mercury (filled block at bottom)
  tft.fillRect(x + 1, y + 13, 13, 2, color);
}

void drawWaterDropletIcon(int x, int y, uint16_t color) {
  // Frame
  tft.drawRect(x, y, 15, 15, color);
  
  // Droplet outline (diamond shape)
  // Left diagonal
  tft.drawLine(x + 7, y + 2, x + 4, y + 7, color);
  // Right diagonal
  tft.drawLine(x + 8, y + 2, x + 11, y + 7, color);
  // Bottom left
  tft.drawLine(x + 4, y + 7, x + 7, y + 13, color);
  // Bottom right
  tft.drawLine(x + 11, y + 7, x + 7, y + 13, color);
  
  // Fill droplet
  tft.fillRect(x + 5, y + 5, 5, 7, color);
}

void drawHeatIcon(int x, int y, uint16_t color) {
  // Frame
  tft.drawRect(x, y, 15, 15, color);
  
  // Center point
  tft.fillRect(x + 6, y + 6, 3, 3, color);
  
  // Cross (vertical + horizontal)
  tft.drawLine(x + 7, y + 2, x + 7, y + 12, color);   // Vertical
  tft.drawLine(x + 2, y + 7, x + 12, y + 7, color);   // Horizontal
  
  // Diagonal points (X shape)
  tft.drawLine(x + 4, y + 4, x + 10, y + 10, color);  // \
  tft.drawLine(x + 10, y + 4, x + 4, y + 10, color);  // /
}

void drawRegionBorder(int region_y_start, int region_y_end, uint16_t color) {
  // Left border line (accent)
  tft.drawLine(2, region_y_start, 2, region_y_end, color);
  
  // Bottom divider line (separates from next region)
  tft.drawLine(0, region_y_end - 1, 159, region_y_end - 1, COLOR_GRID);
}

void drawCardFrame(int y_start, int y_end) {
  // Outer card border (cyan frame)
  tft.drawLine(0, y_start, 159, y_start, COLOR_CYAN);      // Top border
  tft.drawLine(0, y_start, 0, y_end, COLOR_CYAN);          // Left border
  tft.drawLine(0, y_end - 1, 159, y_end - 1, COLOR_CYAN);  // Bottom border
  tft.drawLine(159, y_start, 159, y_end, COLOR_CYAN);      // Right border
}

void drawCornerAccents() {
  // Top-left corner
  tft.drawLine(0, 0, 8, 0, COLOR_CYAN);      // Horizontal
  tft.drawLine(0, 0, 0, 8, COLOR_CYAN);      // Vertical
  
  // Top-right corner
  tft.drawLine(159, 0, 151, 0, COLOR_CYAN);
  tft.drawLine(159, 0, 159, 8, COLOR_CYAN);
  
  // Bottom-left corner
  tft.drawLine(0, 127, 8, 127, COLOR_CYAN);
  tft.drawLine(0, 127, 0, 119, COLOR_CYAN);
  
  // Bottom-right corner
  tft.drawLine(159, 127, 151, 127, COLOR_CYAN);
  tft.drawLine(159, 127, 159, 119, COLOR_CYAN);
}

// ============================================================================
// DISPLAY FUNCTIONS
// ============================================================================

void drawInitialUI() {
  // Fill screen with pure black
  tft.fillScreen(COLOR_BLACK);
  
  // ===== DRAW CARD FRAMES & ICONS =====
  
  // REGION 1: TEMPERATURE (y: 0-42)
  drawCardFrame(0, 42);
  drawThermometerIcon(8, 8, COLOR_COLD);
  
  // REGION 2: HUMIDITY (y: 42-84)
  drawCardFrame(42, 84);
  drawWaterDropletIcon(8, 50, COLOR_CYAN);
  
  // REGION 3: FEELS LIKE (y: 84-128)
  drawCardFrame(84, 128);
  drawHeatIcon(8, 92, COLOR_COLD);
  
  // ===== DRAW CORNER ACCENTS (Cyan) =====
  drawCornerAccents();
  
  // ===== PLACEHOLDER VALUES =====
  updateTemperatureDisplay(0);
  updateHumidityDisplay(0);
  updateFeelsLikeDisplay(0);
}

void updateTemperatureDisplay(float temp) {
  uint16_t color = getTempColor(temp);
  
  // Clear temperature region (rows 0-42)
  tft.fillRect(0, 0, 160, 42, COLOR_BLACK);
  
  // Draw card frame (cyan border)
  drawCardFrame(0, 42);
  
  // Draw thermometer icon
  drawThermometerIcon(8, 8, color);
  
  // Draw large temperature value (text size 3)
  tft.setCursor(40, 5);
  tft.setTextColor(color);
  tft.setTextSize(3);
  tft.printf("%.1f", temp);
  
  // Draw degree symbol (small text size 2)
  tft.setCursor(125, 8);
  tft.setTextColor(color);
  tft.setTextSize(2);
  tft.print("C");
  
  // Status label (COLD, COMFORT, HOT, etc.)
  tft.setCursor(40, 28);
  tft.setTextColor(COLOR_CYAN);
  tft.setTextSize(1);
  if (temp < 15) tft.print("COLD");
  else if (temp < 18) tft.print("COOL");
  else if (temp < 24) tft.print("COMFORT");
  else if (temp < 30) tft.print("WARM");
  else tft.print("HOT!");
  
  // Trend indicator (subtle up/down arrow at bottom)
  if (temp > prev_temperature + 0.5) {
    // Temperature rising
    tft.drawLine(130, 36, 130, 32, COLOR_CYAN);  // Up arrow
    tft.drawLine(127, 35, 130, 32, COLOR_CYAN);
    tft.drawLine(133, 35, 130, 32, COLOR_CYAN);
  } else if (temp < prev_temperature - 0.5) {
    // Temperature falling
    tft.drawLine(130, 32, 130, 36, COLOR_CYAN);  // Down arrow
    tft.drawLine(127, 33, 130, 36, COLOR_CYAN);
    tft.drawLine(133, 33, 130, 36, COLOR_CYAN);
  }
}

void updateHumidityDisplay(float humidity) {
  // Clear humidity region (rows 42-84)
  tft.fillRect(0, 42, 160, 42, COLOR_BLACK);
  
  // Draw card frame (cyan border)
  drawCardFrame(42, 84);
  
  // Draw droplet icon
  drawWaterDropletIcon(8, 50, COLOR_CYAN);
  
  // Draw large humidity value (text size 3)
  tft.setCursor(40, 47);
  tft.setTextColor(COLOR_CYAN);
  tft.setTextSize(3);
  tft.printf("%.0f", humidity);
  
  // Draw percent symbol (text size 2)
  tft.setCursor(130, 50);
  tft.setTextColor(COLOR_CYAN);
  tft.setTextSize(2);
  tft.print("%");
  
  // Horizontal progress bar (humidity level)
  int barWidth = (humidity / 100.0) * 145;
  tft.drawRect(8, 72, 145, 4, COLOR_CYAN);           // Bar outline
  tft.fillRect(8, 72, barWidth, 4, COLOR_CYAN);      // Fill based on %
}

void updateFeelsLikeDisplay(float feels_like) {
  uint16_t color = getTempColor(feels_like);
  
  // Clear feels-like region (rows 84-128)
  tft.fillRect(0, 84, 160, 44, COLOR_BLACK);
  
  // Draw card frame (cyan border)
  drawCardFrame(84, 128);
  
  // Draw heat icon
  drawHeatIcon(8, 92, color);
  
  // Draw large feels-like value (text size 3)
  tft.setCursor(40, 89);
  tft.setTextColor(color);
  tft.setTextSize(3);
  tft.printf("%.1f", feels_like);
  
  // Draw degree symbol (text size 2)
  tft.setCursor(125, 92);
  tft.setTextColor(color);
  tft.setTextSize(2);
  tft.print("C");
  
  // Status label (based on feels-like temperature)
  tft.setCursor(40, 110);
  tft.setTextColor(COLOR_CYAN);
  tft.setTextSize(1);
  if (feels_like < 15) tft.print("COLD FEEL");
  else if (feels_like < 18) tft.print("COOL FEEL");
  else if (feels_like < 24) tft.print("COMFY");
  else if (feels_like < 30) tft.print("WARM FEEL");
  else tft.print("HOT FEEL!");
  
  // Trend indicator (subtle up/down arrow at bottom)
  if (feels_like > prev_feels_like + 0.5) {
    // Feels-like rising
    tft.drawLine(130, 120, 130, 116, COLOR_CYAN);  // Up arrow
    tft.drawLine(127, 119, 130, 116, COLOR_CYAN);
    tft.drawLine(133, 119, 130, 116, COLOR_CYAN);
  } else if (feels_like < prev_feels_like - 0.5) {
    // Feels-like falling
    tft.drawLine(130, 116, 130, 120, COLOR_CYAN);  // Down arrow
    tft.drawLine(127, 117, 130, 120, COLOR_CYAN);
    tft.drawLine(133, 117, 130, 120, COLOR_CYAN);
  }
}

void displaySensorError() {
  // Clear all regions
  tft.fillScreen(COLOR_BLACK);
  
  // Error message
  tft.setCursor(25, 50);
  tft.setTextColor(COLOR_HOT);
  tft.setTextSize(1);
  tft.println("SENSOR ERROR");
  
  tft.setCursor(15, 70);
  tft.setTextColor(COLOR_LIGHT_GRAY);
  tft.setTextSize(1);
  tft.println("Check DHT11");
}

// ============================================================================
// END
// ============================================================================
