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
// COLORS (RGB565)
// ============================================================================

#define COLOR_BLACK      0x0000
#define COLOR_WHITE      0xFFFF
#define COLOR_DARK_GRAY  0x2104
#define COLOR_GRAY       0x4208
#define COLOR_LIGHT_GRAY 0xC618

// Temperature colors (dynamic)
#define COLOR_COLD       0x001F  // Deep blue
#define COLOR_COOL       0x041F  // Blue
#define COLOR_COMFORT    0x07E0  // Green
#define COLOR_WARM       0xFE00  // Orange
#define COLOR_HOT        0xF800  // Red

// Accent colors
#define COLOR_CYAN       0x07FF
#define COLOR_ORANGE     0xFD20

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
  // Heat index formula (Steadman)
  
  if (temp < 26.7) {
    return temp;
  }
  
  if (humidity < 0 || humidity > 100) {
    return temp;
  }
  
  double T = temp;
  double RH = humidity;
  
  double c1 = -42.379;
  double c2 = 2.04901523;
  double c3 = 10.14333127;
  double c4 = -0.22475541;
  double c5 = -0.00683783;
  double c6 = -0.05481717;
  double c7 = 0.00122874;
  double c8 = 0.00085282;
  double c9 = -0.00000199;
  
  double HI = c1 + c2 * T + c3 * RH + c4 * T * RH + c5 * T * T
            + c6 * RH * RH + c7 * T * T * RH + c8 * T * RH * RH 
            + c9 * T * T * RH * RH;
  
  if (HI < T) return T;
  if (HI > 60) return 60.0;
  
  return (float)HI;
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
// DISPLAY FUNCTIONS
// ============================================================================

void drawInitialUI() {
  tft.fillScreen(COLOR_BLACK);
  
  // Draw subtle top border
  tft.drawLine(0, 0, 160, 0, COLOR_GRAY);
  
  // Title
  tft.setCursor(5, 8);
  tft.setTextColor(COLOR_LIGHT_GRAY);
  tft.setTextSize(1);
  tft.println("CLIMATE MONITOR");
  
  tft.drawLine(0, 20, 160, 20, COLOR_DARK_GRAY);
  
  // Placeholder display
  updateTemperatureDisplay(0);
  updateHumidityDisplay(0);
  updateFeelsLikeDisplay(0);
}

void updateTemperatureDisplay(float temp) {
  uint16_t color = getTempColor(temp);
  
  // Clear temperature region (region 1: rows 25-65)
  tft.fillRect(0, 25, 160, 42, COLOR_BLACK);
  
  // Draw temperature label
  tft.setCursor(8, 28);
  tft.setTextColor(COLOR_LIGHT_GRAY);
  tft.setTextSize(1);
  tft.print("TEMPERATURE");
  
  // Draw large temperature value
  tft.setCursor(18, 40);
  tft.setTextColor(color);
  tft.setTextSize(4);
  tft.printf("%.1f", temp);
  
  // Draw degree symbol and unit
  tft.setCursor(128, 42);
  tft.setTextColor(color);
  tft.setTextSize(2);
  tft.print("o");
  
  tft.setCursor(145, 48);
  tft.setTextColor(color);
  tft.setTextSize(1);
  tft.print("C");
}

void updateHumidityDisplay(float humidity) {
  // Clear humidity region (region 2: rows 68-100)
  tft.fillRect(0, 68, 160, 30, COLOR_BLACK);
  
  // Draw humidity label
  tft.setCursor(8, 70);
  tft.setTextColor(COLOR_LIGHT_GRAY);
  tft.setTextSize(1);
  tft.print("HUMIDITY");
  
  // Draw humidity value
  tft.setCursor(70, 73);
  tft.setTextColor(COLOR_CYAN);
  tft.setTextSize(3);
  tft.printf("%.0f", humidity);
  
  tft.setCursor(130, 76);
  tft.setTextColor(COLOR_CYAN);
  tft.setTextSize(1);
  tft.print("%");
  
  // Draw humidity bar
  int bar_width = (int)(humidity / 100.0 * 140);
  tft.fillRect(8, 92, bar_width, 4, COLOR_CYAN);
  tft.drawRect(8, 92, 140, 4, COLOR_LIGHT_GRAY);
}

void updateFeelsLikeDisplay(float feels_like) {
  // Clear feels-like region (region 3: rows 103-128)
  tft.fillRect(0, 103, 160, 25, COLOR_BLACK);
  
  // Draw feels-like label
  tft.setCursor(8, 105);
  tft.setTextColor(COLOR_LIGHT_GRAY);
  tft.setTextSize(1);
  tft.print("FEELS LIKE");
  
  // Draw feels-like value
  uint16_t fl_color = getTempColor(feels_like);
  tft.setCursor(65, 110);
  tft.setTextColor(fl_color);
  tft.setTextSize(2);
  tft.printf("%.1f", feels_like);
  
  tft.setCursor(125, 111);
  tft.setTextColor(fl_color);
  tft.setTextSize(1);
  tft.print("oC");
}

void displaySensorError() {
  // Clear middle area
  tft.fillRect(0, 25, 160, 103, COLOR_BLACK);
  
  // Error message
  tft.setCursor(30, 60);
  tft.setTextColor(COLOR_HOT);
  tft.setTextSize(1);
  tft.println("SENSOR ERROR");
  
  tft.setCursor(15, 75);
  tft.setTextColor(COLOR_LIGHT_GRAY);
  tft.setTextSize(1);
  tft.println("Check DHT11 connection");
}

// ============================================================================
// END
// ============================================================================
