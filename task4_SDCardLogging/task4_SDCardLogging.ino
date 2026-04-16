/*
 * ============================================================================
 * PROJECT: espFrameBuffer - Task 4: SD Card CSV Logging
 * ============================================================================
 * PURPOSE:
 *   Log temperature, humidity, and calculated feels-like temperature to SD card
 *   in CSV format with ISO 8601 timestamps. Display data on TFT LCD in landscape.
 *   Share SPI bus between display and SD card.
 *
 * HARDWARE:
 *   - ESP32 or ESP32-C3
 *   - 1.8" TFT LCD (128x160 pixels) with ST7735s controller
 *   - DHT11 sensor (temperature + humidity)
 *   - SD card module (SPI interface, shared bus with TFT)
 *   - WiFi for NTP time sync at startup
 *
 * REQUIRED LIBRARIES:
 *   - Adafruit_ST7735
 *   - Adafruit_GFX
 *   - Adafruit_DHT
 *   - SD.h (SPI SD card library)
 *   - WiFi.h (Built-in)
 *   - time.h (Built-in)
 *
 * SHARED SPI BUS:
 *   Both TFT LCD and SD card share same SPI pins:
 *   - MOSI: GPIO 23 (ESP32) / GPIO 6 (ESP32-C3)
 *   - SCK: GPIO 18 (ESP32) / GPIO 4 (ESP32-C3)
 *   Each has separate CS pin to prevent conflicts.
 *
 * CSV FORMAT:
 *   timestamp,temperature,humidity,feels_like
 *   2026-04-18T14:32:45Z,25.3,65.0,28.5
 *
 * ============================================================================
 */

#include <Adafruit_ST7735.h>
#include <Adafruit_GFX.h>
#include <DHT.h>
#include <SD.h>
#include <SPI.h>
#include <WiFi.h>
#include <time.h>

// ============================================================================
// CONFIGURATION (EDIT THESE)
// ============================================================================

#define SSID              "Itzz_Infinity"
#define PASSWORD          "123456789"
// #define LOGGING_INTERVAL  300000        // Log every 5 minutes (300000 ms)
#define LOGGING_INTERVAL  10        // Log every 5 minutes (300000 ms)
#define NTP_SERVER        "pool.ntp.org"
#define GMT_OFFSET        19800          // UTC+5:30

// ============================================================================
// COLORS
// ============================================================================

#define COLOR_BLACK   0x0000
#define COLOR_WHITE   0xFFFF
#define COLOR_CYAN    0x07FF
#define COLOR_GREEN   0x07E0
#define COLOR_YELLOW  0xFFE0
#define COLOR_RED     0xF800
#define COLOR_ORANGE  0xFD20

// ============================================================================
// DHT11 CONFIGURATION
// ============================================================================

#define DHT_PIN      14
#define DHT_TYPE     DHT11

DHT dht(DHT_PIN, DHT_TYPE);

// ============================================================================
// PIN DEFINITIONS (Board-specific)
// ============================================================================

#if defined(ESP32) && !defined(CONFIG_IDF_TARGET_ESP32C3)
  #define TFT_CS   5
  #define TFT_RST  4
  #define TFT_DC   2
  #define TFT_MOSI 23
  #define TFT_SCK  18
  #define SD_CS    17      // SD card chip select (different from TFT)
#elif defined(CONFIG_IDF_TARGET_ESP32C3) || defined(ESP32C3)
  #define TFT_CS   7
  #define TFT_RST  8
  #define TFT_DC   10
  #define TFT_MOSI 6
  #define TFT_SCK  4
  #define SD_CS    3       // SD card chip select (different from TFT)
#endif

// ============================================================================
// GLOBALS
// ============================================================================

Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_MOSI, TFT_SCK, TFT_RST);

float prev_temperature = -999;
float prev_humidity = -999;
float prev_feels_like = -999;

unsigned long last_log_time = 0;
unsigned long last_dht_read = 0;

bool sd_ready = false;
bool time_synced = false;

const char* csv_filename = "data_log.csv";

#define DHT_READ_INTERVAL 2000

// ============================================================================
// SETUP
// ============================================================================

void setup() {
  Serial.begin(115200);
  delay(2000);
  
  Serial.println("\n\nTask 4: SD Card CSV Logging");
  
  // Initialize display in landscape
  tft.initR(INITR_BLACKTAB);
  tft.setRotation(1);  // Landscape orientation
  tft.fillScreen(COLOR_BLACK);
  
  Serial.println("Display initialized (landscape mode)");
  
  // Initialize DHT11
  dht.begin();
  Serial.println("DHT11 initialized on GPIO " + String(DHT_PIN));
  
  // Display startup message
  drawMessage("Initializing", "WiFi & SD Card", COLOR_YELLOW);
  
  // Sync time via WiFi
  syncTime();
  
  // Initialize SD card
  initSDCard();
  
  // Create CSV header if file doesn't exist
  if (sd_ready) {
    createCSVHeader();
  }
  
  displayInit();
}

// ============================================================================
// MAIN LOOP
// ============================================================================

void loop() {
  unsigned long now = millis();
  
  // Read DHT11 every 2 seconds
  if (now - last_dht_read >= DHT_READ_INTERVAL) {
    last_dht_read = now;
    readAndDisplayDHT();
  }
  
  // Log to SD card at interval
  if (now - last_log_time >= LOGGING_INTERVAL) {
    last_log_time = now;
    logToSD();
  }
  
  delay(100);
}

// ============================================================================
// TIME FUNCTIONS
// ============================================================================

void syncTime() {
  Serial.print("Connecting to WiFi: ");
  Serial.println(SSID);
  
  drawMessage("WiFi", "Connecting...", COLOR_YELLOW);
  
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
    drawMessage("WiFi Error", "Using default time", COLOR_RED);
    delay(2000);
    return;
  }
  
  Serial.println("\n✓ WiFi Connected");
  
  // Sync time
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
  
  WiFi.disconnect(true);
  Serial.println("WiFi disconnected");
}

void getISO8601Timestamp(char* buffer, size_t buffer_size) {
  time_t now = time(nullptr);
  struct tm* timeinfo = localtime(&now);
  
  snprintf(buffer, buffer_size, "%04d-%02d-%02dT%02d:%02d:%02dZ",
           1900 + timeinfo->tm_year,
           1 + timeinfo->tm_mon,
           timeinfo->tm_mday,
           timeinfo->tm_hour,
           timeinfo->tm_min,
           timeinfo->tm_sec);
}

// ============================================================================
// SD CARD FUNCTIONS
// ============================================================================

void initSDCard() {
  Serial.println("Initializing SD card...");
  drawMessage("SD Card", "Initializing...", COLOR_YELLOW);
  
  if (!SD.begin(SD_CS)) {
    Serial.println("✗ SD card initialization failed!");
    drawMessage("SD Error", "Card not detected", COLOR_RED);
    sd_ready = false;
    delay(2000);
    return;
  }
  
  Serial.println("✓ SD card initialized");
  sd_ready = true;
}

void createCSVHeader() {
  // Check if file exists
  if (SD.exists(csv_filename)) {
    Serial.println("CSV file already exists, skipping header");
    return;
  }
  
  // Create new file with header
  File file = SD.open(csv_filename, FILE_WRITE);
  if (file) {
    file.println("timestamp,temperature,humidity,feels_like");
    file.close();
    Serial.println("✓ CSV header created");
  } else {
    Serial.println("✗ Failed to create CSV file");
  }
}

void logToSD() {
  if (!sd_ready) {
    Serial.println("✗ SD card not ready");
    displaySDError("SD Not Ready");
    return;
  }
  
  // Read current sensor values
  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();
  
  if (isnan(temperature) || isnan(humidity)) {
    Serial.println("✗ Failed to read sensor");
    displaySDError("Sensor Error");
    return;
  }
  
  // Calculate feels like temperature
  float feels_like = calculateFeelsLike(temperature, humidity);
  
  // Get ISO 8601 timestamp
  char timestamp[30];
  getISO8601Timestamp(timestamp, sizeof(timestamp));
  
  // Open file and append
  File file = SD.open(csv_filename, FILE_APPEND);
  if (!file) {
    Serial.println("✗ Failed to open CSV file");
    displaySDError("File Error");
    return;
  }
  
  // Format and write CSV line
  char csv_line[100];
  snprintf(csv_line, sizeof(csv_line), "%s,%.1f,%.1f,%.1f",
           timestamp, temperature, humidity, feels_like);
  
  file.println(csv_line);
  file.close();
  
  Serial.print("✓ Logged: ");
  Serial.println(csv_line);
  
  displayLastLog(timestamp);
}

// ============================================================================
// SENSOR FUNCTIONS
// ============================================================================

void readAndDisplayDHT() {
  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();
  
  if (isnan(temperature) || isnan(humidity)) {
    Serial.println("✗ Failed to read DHT11");
    return;
  }
  
  float feels_like = calculateFeelsLike(temperature, humidity);
  
  // Only update display if values changed
  if (abs(temperature - prev_temperature) >= 0.5 ||
      abs(humidity - prev_humidity) >= 2.0 ||
      abs(feels_like - prev_feels_like) >= 0.5) {
    
    displaySensorData(temperature, humidity, feels_like);
    
    prev_temperature = temperature;
    prev_humidity = humidity;
    prev_feels_like = feels_like;
    
    Serial.printf("T:%.1f H:%.1f F:%.1f\n", temperature, humidity, feels_like);
  }
}

float calculateFeelsLike(float temp, float humidity) {
  // Heat index formula (Steadman)
  // Only relevant when temperature > ~27°C
  
  if (temp < 26.7) {
    return temp;  // At lower temps, feels like = actual temp
  }
  
  double T = temp;
  double RH = humidity;
  
  // Steadman formula
  double HI = -42.379 + (2.04901523 * T) + (10.14333127 * RH)
            - (0.22475541 * T * RH) - (0.00683783 * T * T)
            - (0.05481717 * RH * RH) + (0.00122874 * T * T * RH)
            + (0.00085282 * T * RH * RH) - (0.00000199 * T * T * RH * RH);
  
  return (float)HI;
}

// ============================================================================
// DISPLAY FUNCTIONS
// ============================================================================

void displayInit() {
  tft.fillScreen(COLOR_BLACK);
  
  // Header
  tft.setCursor(5, 5);
  tft.setTextColor(COLOR_CYAN);
  tft.setTextSize(1);
  tft.println("Temperature | Humidity | Feels Like");
  
  tft.drawLine(0, 18, 160, 18, COLOR_CYAN);
  
  // Placeholder values
  displaySensorData(0, 0, 0);
}

void displaySensorData(float temp, float humidity, float feels_like) {
  // Clear sensor data region (partial update)
  tft.fillRect(0, 22, 160, 35, COLOR_BLACK);
  
  // Temperature
  tft.setCursor(5, 25);
  tft.setTextColor(COLOR_GREEN);
  tft.setTextSize(2);
  tft.printf("%.1f", temp);
  tft.setTextSize(1);
  tft.print("C");
  
  // Humidity
  tft.setCursor(58, 25);
  tft.setTextColor(COLOR_ORANGE);
  tft.setTextSize(2);
  tft.printf("%.0f", humidity);
  tft.setTextSize(1);
  tft.print("%");
  
  // Feels Like
  tft.setCursor(105, 25);
  tft.setTextColor(COLOR_YELLOW);
  tft.setTextSize(2);
  tft.printf("%.1f", feels_like);
  tft.setTextSize(1);
  tft.print("C");
}

void displayLastLog(const char* timestamp) {
  // Clear status region (partial update)
  tft.fillRect(0, 62, 160, 35, COLOR_BLACK);
  
  tft.drawLine(0, 60, 160, 60, COLOR_CYAN);
  
  tft.setCursor(5, 65);
  tft.setTextColor(COLOR_WHITE);
  tft.setTextSize(1);
  tft.print("Last Log: ");
  tft.println(timestamp);
  
  tft.setCursor(5, 78);
  tft.setTextColor(COLOR_GREEN);
  tft.print("Interval: ");
  tft.print(LOGGING_INTERVAL / 1000);
  tft.println("s");
  
  tft.setCursor(5, 91);
  tft.setTextColor(COLOR_GREEN);
  tft.println("Status: OK");
}

void displaySDError(const char* error) {
  // Clear status region
  tft.fillRect(0, 62, 160, 35, COLOR_BLACK);
  
  tft.drawLine(0, 60, 160, 60, COLOR_RED);
  
  tft.setCursor(5, 70);
  tft.setTextColor(COLOR_RED);
  tft.setTextSize(1);
  tft.println("SD ERROR:");
  
  tft.setCursor(5, 85);
  tft.setTextColor(COLOR_YELLOW);
  tft.println(error);
}

void drawMessage(const char* title, const char* message, uint16_t color) {
  tft.fillScreen(COLOR_BLACK);
  
  tft.setCursor(20, 50);
  tft.setTextColor(color);
  tft.setTextSize(2);
  tft.println(title);
  
  tft.setCursor(15, 85);
  tft.setTextColor(COLOR_WHITE);
  tft.setTextSize(1);
  tft.println(message);
}

// ============================================================================
// END
// ============================================================================
