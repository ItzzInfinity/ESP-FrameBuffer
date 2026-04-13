/*
 * ============================================================================
 * PROJECT: espFrameBuffer - Task 2: Simple Internet Clock
 * ============================================================================
 * PURPOSE:
 *   Connect to WiFi once, sync time via NTP, display date/time using internal
 *   clock. Minimal code, no network scanning, no refresh spam.
 *
 * HARDWARE:
 *   - ESP32 or ESP32-C3
 *   - 1.8" TFT LCD (128x160 pixels) with ST7735s controller
 *   - WiFi for NTP time sync only
 *
 * REQUIRED LIBRARIES:
 *   - Adafruit_ST7735
 *   - Adafruit_GFX
 *   - WiFi.h (Built-in)
 *   - time.h (Built-in)
 *
 * ============================================================================
 */

#include <Adafruit_ST7735.h>
#include <Adafruit_GFX.h>
#include <SPI.h>
#include <WiFi.h>
#include <time.h>

// ============================================================================
// COLORS
// ============================================================================

#define COLOR_BLACK   0x0000
#define COLOR_WHITE   0xFFFF
#define COLOR_CYAN    0x07FF
#define COLOR_YELLOW  0xFFE0
#define COLOR_GREEN   0x07E0

// ============================================================================
// CONFIGURATION
// ============================================================================

#define SSID         "Itzz_Infinity"
#define PASSWORD     "123456789"
#define NTP_SERVER   "pool.ntp.org"
#define GMT_OFFSET   19800           // UTC+5:30 (India Standard Time)
#define DST_OFFSET   0
#define REFRESH_INTERVAL 60000        // Update display every 60 seconds

// ============================================================================
// PINS
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

bool time_synced = false;
unsigned long last_display_update = 0;

// ============================================================================
// SETUP
// ============================================================================

void setup() {
  Serial.begin(115200);
  delay(2000);
  
  Serial.println("\n\nTask 2: Simple Internet Clock");
  
  // Initialize display
  tft.initR(INITR_BLACKTAB);
  tft.setRotation(0);
  tft.fillScreen(COLOR_BLACK);
  
  drawText("Connecting...", 20, 60, COLOR_YELLOW, 1);
  
  // Connect to WiFi and sync time
  connectAndSyncTime();
  
  // Initial display
  if (time_synced) {
    displayTime();
  } else {
    drawText("Time Sync Failed", 15, 60, COLOR_YELLOW, 1);
  }
}

// ============================================================================
// MAIN LOOP
// ============================================================================

void loop() {
  if (time_synced) {
    if (millis() - last_display_update >= REFRESH_INTERVAL) {
      last_display_update = millis();
      displayTime();
    }
  }
  delay(1000);
}

// ============================================================================
// FUNCTIONS
// ============================================================================

void connectAndSyncTime() {
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
    return;
  }
  
  Serial.println("\n✓ WiFi Connected");
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());
  
  // Sync time
  Serial.println("Syncing time via NTP...");
  configTime(GMT_OFFSET, DST_OFFSET, NTP_SERVER);
  
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

void displayTime() {
  tft.fillScreen(COLOR_BLACK);
  
  time_t now = time(nullptr);
  struct tm* timeinfo = localtime(&now);
  
  // Header
  tft.setCursor(15, 5);
  tft.setTextColor(COLOR_CYAN);
  tft.setTextSize(1);
  tft.println("INTERNET CLOCK");
  
  // Separator
  tft.drawLine(5, 18, 123, 18, COLOR_CYAN);
  
  // Big Time (HH:MM format - no seconds)
  tft.setCursor(15, 45);
  tft.setTextColor(COLOR_YELLOW);
  tft.setTextSize(3);
  
  char time_str[6];
  sprintf(time_str, "%02d:%02d", timeinfo->tm_hour, timeinfo->tm_min);
  tft.println(time_str);
  
  // Date
  tft.setCursor(10, 105);
  tft.setTextColor(COLOR_WHITE);
  tft.setTextSize(1);
  
  const char* days[] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};
  const char* months[] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", 
                          "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
  
  char date_str[30];
  sprintf(date_str, "%s, %s %d, %d", 
          days[timeinfo->tm_wday],
          months[timeinfo->tm_mon],
          timeinfo->tm_mday,
          1900 + timeinfo->tm_year);
  
  tft.println(date_str);
  
  // Footer
  tft.drawLine(5, 130, 123, 130, COLOR_GREEN);
  tft.setCursor(10, 140);
  tft.setTextColor(COLOR_GREEN);
  tft.setTextSize(1);
  tft.println("Updates every 1 min");
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
