/*
 * ============================================================================
 * PROJECT: espFrameBuffer - Task 3.1: DHT11 + Internet Time
 * ============================================================================
 * PURPOSE:
 *   Display temperature/humidity from DHT11 sensor AND internet time on same
 *   TFT LCD screen. Uses PARTIAL SCREEN UPDATES to avoid flickering.
 *   Only updates regions that changed (intelligent refresh).
 *
 * KEY FEATURES:
 *   - DHT11 reads every 2 seconds (DHT11 minimum)
 *   - Only updates display if temperature or humidity CHANGED
 *   - Time synced via NTP at startup, uses internal clock after
 *   - Time updates every 60 seconds (no seconds shown)
 *   - Partial updates: Only refreshes changed regions, not entire display
 *
 * HARDWARE:
 *   - ESP32 or ESP32-C3
 *   - 1.8" TFT LCD (128x160 pixels) with ST7735s controller
 *   - DHT11 sensor (temperature + humidity)
 *   - WiFi for initial NTP time sync
 *
 * REQUIRED LIBRARIES:
 *   - Adafruit_ST7735
 *   - Adafruit_GFX
 *   - Adafruit_DHT
 *   - WiFi.h (Built-in)
 *   - time.h (Built-in)
 *
 * DISPLAY LAYOUT (128x160):
 *   ┌──────────────────────────┐
 *   │ TIME: 14:32              │  ← Time Region (0-35px)
 *   ├──────────────────────────┤
 *   │ Temp: 25.3°C             │  ← Temp Region (35-95px)
 *   │                          │
 *   │                          │
 *   ├──────────────────────────┤
 *   │ Humidity: 65%            │  ← Humidity Region (95-160px)
 *   │                          │
 *   └──────────────────────────┘
 *
 * ============================================================================
 */

#include <Adafruit_ST7735.h>
#include <Adafruit_GFX.h>
#include <DHT.h>
#include <SPI.h>
#include <WiFi.h>
#include <time.h>

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
// CONFIGURATION
// ============================================================================

#define SSID         "Itzz_Infinity"
#define PASSWORD     "123456789"
#define NTP_SERVER   "pool.ntp.org"
#define GMT_OFFSET   19800           // UTC+5:30

#define DHT_PIN      14              // GPIO pin for DHT11 DATA
#define DHT_TYPE     DHT11

#define DHT_READ_INTERVAL    2000    // Read sensor every 2 seconds
#define TIME_UPDATE_INTERVAL 60000   // Update time every 60 seconds

// ============================================================================
// DISPLAY REGION DEFINITIONS (Partial Updates)
// ============================================================================

#define TIME_REGION_Y 0
#define TIME_REGION_H 35

#define TEMP_REGION_Y 35
#define TEMP_REGION_H 60

#define HUMID_REGION_Y 95
#define HUMID_REGION_H 65

// ============================================================================
// PIN DEFINITIONS
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
DHT dht(DHT_PIN, DHT_TYPE);

// State tracking
float prev_temperature = -999;
float prev_humidity = -999;
int prev_hour = -1;
int prev_minute = -1;

unsigned long last_dht_read = 0;
unsigned long last_time_update = 0;

bool time_synced = false;

// ============================================================================
// SETUP
// ============================================================================

void setup() {
  Serial.begin(115200);
  delay(2000);
  
  Serial.println("\n\nTask 3.1: DHT11 Sensor + Internet Time");
  
  // Initialize display
  tft.initR(INITR_BLACKTAB);
  tft.setRotation(0);
  tft.fillScreen(COLOR_BLACK);
  
  Serial.println("Display initialized");
  
  // Initialize DHT11
  dht.begin();
  Serial.println("DHT11 initialized on GPIO " + String(DHT_PIN));
  
  // Connect to WiFi and sync time
  drawFullScreenMessage("Syncing Time", "Please Wait...");
  syncTime();
  
  // Initial display
  updateTimeDisplay(true);
  delay(1000);
}

// ============================================================================
// MAIN LOOP
// ============================================================================

void loop() {
  unsigned long now = millis();
  
  // Read DHT11 sensor every 2 seconds (minimum for DHT11)
  if (now - last_dht_read >= DHT_READ_INTERVAL) {
    last_dht_read = now;
    readAndUpdateDHT();
  }
  
  // Update time display every 60 seconds (only if changed)
  if (now - last_time_update >= TIME_UPDATE_INTERVAL) {
    last_time_update = now;
    updateTimeDisplay(false);
  }
  
  delay(100);
}

// ============================================================================
// TIME FUNCTIONS
// ============================================================================

void syncTime() {
  Serial.print("Connecting to WiFi: ");
  Serial.println(SSID);
  
  WiFi.mode(WIFI_STA);
  WiFi.begin(SSID, PASSWORD);
  
  int timeout = 20;
  while (WiFi.status() != WL_CONNECTED && timeout > 0) {
    delay(500);
    Serial.print(".");
    timeout--;
  }
  
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("\n✗ WiFi Connection Failed");
    drawFullScreenMessage("WiFi Error", "Using default time");
    return;
  }
  
  Serial.println("\n✓ WiFi Connected");
  
  // Sync time via NTP
  Serial.println("Syncing time via NTP...");
  configTime(GMT_OFFSET, 0, NTP_SERVER);
  
  time_t now = time(nullptr);
  timeout = 20;
  while (now < 24 * 3600 && timeout > 0) {
    delay(500);
    Serial.print(".");
    now = time(nullptr);
    timeout--;
  }
  
  Serial.println();
  if (now > 24 * 3600) {
    Serial.print("✓ Time synced: ");
    Serial.println(ctime(&now));
    time_synced = true;
  } else {
    Serial.println("✗ NTP Sync Failed");
  }
  
  // Disconnect WiFi to save power
  WiFi.disconnect(true);
  Serial.println("WiFi disconnected (using internal clock)");
}

void updateTimeDisplay(bool force_update) {
  time_t now = time(nullptr);
  struct tm* timeinfo = localtime(&now);
  
  int current_hour = timeinfo->tm_hour;
  int current_minute = timeinfo->tm_min;
  
  // Only update if time changed OR forced at startup
  if (!force_update && (current_hour == prev_hour && current_minute == prev_minute)) {
    return;  // No change, skip update
  }
  
  // Update time region only (partial update)
  tft.fillRect(0, TIME_REGION_Y, 128, TIME_REGION_H, COLOR_BLACK);
  
  // Draw header
  tft.setCursor(10, 5);
  tft.setTextColor(COLOR_CYAN);
  tft.setTextSize(1);
  tft.println("Internet Time");
  
  // Draw large time (HH:MM format)
  tft.setCursor(15, 15);
  tft.setTextColor(COLOR_YELLOW);
  tft.setTextSize(2);
  
  char time_str[6];
  sprintf(time_str, "%02d:%02d", current_hour, current_minute);
  tft.println(time_str);
  
  // Separator line
  tft.drawLine(5, 33, 123, 33, COLOR_CYAN);
  
  prev_hour = current_hour;
  prev_minute = current_minute;
  
  Serial.printf("Time updated: %02d:%02d\n", current_hour, current_minute);
}

// ============================================================================
// DHT11 FUNCTIONS
// ============================================================================

void readAndUpdateDHT() {
  float humidity = dht.readHumidity();
  float temperature = dht.readTemperature();
  
  // Validate reading
  if (isnan(humidity) || isnan(temperature)) {
    Serial.println("✗ Failed to read from DHT11");
    return;
  }
  
  // Check if values changed (with tolerance for sensor noise)
  bool temp_changed = (abs(temperature - prev_temperature) >= 0.5);
  bool humid_changed = (abs(humidity - prev_humidity) >= 2.0);
  
  if (!temp_changed && !humid_changed) {
    return;  // No significant change, skip update
  }
  
  // Update temperature display if changed
  if (temp_changed) {
    updateTempDisplay(temperature);
    prev_temperature = temperature;
  }
  
  // Update humidity display if changed
  if (humid_changed) {
    updateHumidDisplay(humidity);
    prev_humidity = humidity;
  }
  
  Serial.printf("Temp: %.1f°C, Humidity: %.0f%%\n", temperature, humidity);
}

void updateTempDisplay(float temperature) {
  // Clear temp region only
  tft.fillRect(0, TEMP_REGION_Y, 128, TEMP_REGION_H, COLOR_BLACK);
  
  // Draw separator
  tft.drawLine(5, TEMP_REGION_Y, 123, TEMP_REGION_Y, COLOR_CYAN);
  
  // Label
  tft.setCursor(10, TEMP_REGION_Y + 8);
  tft.setTextColor(COLOR_GREEN);
  tft.setTextSize(1);
  tft.println("Temperature");
  
  // Value
  tft.setCursor(20, TEMP_REGION_Y + 25);
  tft.setTextColor(COLOR_GREEN);
  tft.setTextSize(3);
  tft.print(temperature, 1);  // 1 decimal
  tft.setTextSize(1);
  tft.println(" C");
}

void updateHumidDisplay(float humidity) {
  // Clear humidity region only
  tft.fillRect(0, HUMID_REGION_Y, 128, HUMID_REGION_H, COLOR_BLACK);
  
  // Draw separator
  tft.drawLine(5, HUMID_REGION_Y, 123, HUMID_REGION_Y, COLOR_CYAN);
  
  // Label
  tft.setCursor(10, HUMID_REGION_Y + 8);
  tft.setTextColor(COLOR_ORANGE);
  tft.setTextSize(1);
  tft.println("Humidity");
  
  // Value
  tft.setCursor(30, HUMID_REGION_Y + 25);
  tft.setTextColor(COLOR_ORANGE);
  tft.setTextSize(3);
  tft.print((int)humidity);  // No decimal
  tft.setTextSize(1);
  tft.println(" %");
}

// ============================================================================
// HELPER FUNCTIONS
// ============================================================================

void drawFullScreenMessage(const char* line1, const char* line2) {
  tft.fillScreen(COLOR_BLACK);
  
  tft.setCursor(20, 60);
  tft.setTextColor(COLOR_YELLOW);
  tft.setTextSize(1);
  tft.println(line1);
  
  tft.setCursor(20, 80);
  tft.setTextColor(COLOR_WHITE);
  tft.setTextSize(1);
  tft.println(line2);
}

// ============================================================================
// END
// ============================================================================
