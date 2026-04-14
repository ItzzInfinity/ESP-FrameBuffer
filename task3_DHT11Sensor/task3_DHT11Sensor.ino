/*
 * ============================================================================
 * PROJECT: espFrameBuffer - Task 3: DHT11 Temperature & Humidity Sensor
 * ============================================================================
 * PURPOSE:
 *   Read temperature and humidity from DHT11 sensor and display on TFT LCD.
 *   Simple, minimal implementation with clean display formatting.
 *
 * HARDWARE:
 *   - ESP32 or ESP32-C3
 *   - 1.8" TFT LCD (128x160 pixels) with ST7735s controller
 *   - DHT11 sensor (temperature + humidity)
 *
 * REQUIRED LIBRARIES:
 *   - Adafruit_ST7735
 *   - Adafruit_GFX
 *   - Adafruit_DHT (Adafruit DHT Sensor Library)
 *   - SPI.h (Built-in)
 *
 * WIRING:
 *   DHT11 Pin 1 (VCC) → 3.3V
 *   DHT11 Pin 2 (DATA) → GPIO pin (see below)
 *   DHT11 Pin 3 (NC) → Not connected
 *   DHT11 Pin 4 (GND) → GND
 *
 * ============================================================================
 */

#include <Adafruit_ST7735.h>
#include <Adafruit_GFX.h>
#include <DHT.h>
#include <SPI.h>

// ============================================================================
// COLORS
// ============================================================================

#define COLOR_BLACK   0x0000
#define COLOR_WHITE   0xFFFF
#define COLOR_CYAN    0x07FF
#define COLOR_GREEN   0x07E0
#define COLOR_YELLOW  0xFFE0
#define COLOR_ORANGE  0xFD20

// ============================================================================
// DHT11 CONFIGURATION
// ============================================================================

#define DHTTYPE DHT11
#define DHT_PIN 14           // GPIO pin for DHT11 DATA

DHT dht(DHT_PIN, DHTTYPE);

// ============================================================================
// TFT PINS
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

float temperature = 0;
float humidity = 0;
unsigned long last_read = 0;

#define READ_INTERVAL 2000  // Read sensor every 2 seconds (DHT11 minimum)

// ============================================================================
// SETUP
// ============================================================================

void setup() {
  Serial.begin(115200);
  delay(2000);
  
  Serial.println("\n\nTask 3: DHT11 Sensor Display");
  
  // Initialize display
  tft.initR(INITR_BLACKTAB);
  tft.setRotation(0);
  tft.fillScreen(COLOR_BLACK);
  
  Serial.println("Display initialized");
  
  // Initialize DHT11
  dht.begin();
  Serial.println("DHT11 initialized on GPIO " + String(DHT_PIN));
  
  drawText("Initializing", 30, 60, COLOR_YELLOW, 1);
  drawText("DHT11 Sensor", 25, 75, COLOR_YELLOW, 1);
  
  delay(2000);
  
  // First reading
  readSensor();
  displayData();
}

// ============================================================================
// MAIN LOOP
// ============================================================================

void loop() {
  if (millis() - last_read >= READ_INTERVAL) {
    last_read = millis();
    readSensor();
    displayData();
  }
  
  delay(100);
}

// ============================================================================
// FUNCTIONS
// ============================================================================

void readSensor() {
  humidity = dht.readHumidity();
  temperature = dht.readTemperature();
  
  // Check if reading failed
  if (isnan(humidity) || isnan(temperature)) {
    Serial.println("✗ Failed to read from DHT11!");
    temperature = -1;
    humidity = -1;
  } else {
    Serial.print("Temperature: ");
    Serial.print(temperature);
    Serial.print("°C, Humidity: ");
    Serial.print(humidity);
    Serial.println("%");
  }
}

void displayData() {
  tft.fillScreen(COLOR_BLACK);
  
  // Header
  tft.setCursor(15, 8);
  tft.setTextColor(COLOR_CYAN);
  tft.setTextSize(1);
  tft.println("DHT11 SENSOR");
  
  // Separator
  tft.drawLine(5, 20, 123, 20, COLOR_CYAN);
  
  // Check if reading is valid
  if (temperature == -1 || humidity == -1) {
    tft.setCursor(15, 70);
    tft.setTextColor(COLOR_YELLOW);
    tft.setTextSize(1);
    tft.println("Sensor Error!");
    tft.setCursor(10, 95);
    tft.println("Check wiring");
    return;
  }
  
  // Temperature Section
  tft.setCursor(10, 35);
  tft.setTextColor(COLOR_GREEN);
  tft.setTextSize(1);
  tft.println("Temperature");
  
  tft.setCursor(20, 55);
  tft.setTextColor(COLOR_GREEN);
  tft.setTextSize(3);
  tft.print(temperature, 1);  // 1 decimal place
  tft.setTextSize(1);
  tft.println(" C");
  
  // Separator
  tft.drawLine(5, 85, 123, 85, COLOR_CYAN);
  
  // Humidity Section
  tft.setCursor(10, 100);
  tft.setTextColor(COLOR_ORANGE);
  tft.setTextSize(1);
  tft.println("Humidity");
  
  tft.setCursor(30, 120);
  tft.setTextColor(COLOR_ORANGE);
  tft.setTextSize(3);
  tft.print(humidity, 0);  // No decimal places
  tft.setTextSize(1);
  tft.println(" %");
  
  // Footer
  tft.drawLine(5, 148, 123, 148, COLOR_GREEN);
  tft.setCursor(15, 153);
  tft.setTextColor(COLOR_WHITE);
  tft.setTextSize(1);
  tft.println("Updates every 2s");
}

void drawText(const char* text, int x, int y, uint16_t color, uint8_t size) {
  tft.setCursor(x, y);
  tft.setTextColor(color);
  tft.setTextSize(size);
  tft.println(text);
}

// ============================================================================
// END
// ============================================================================
