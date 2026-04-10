/*
 * ============================================================================
 * PROJECT: espFrameBuffer - Task 2: Internet Time Display
 * ============================================================================
 * PURPOSE:
 *   Connect to WiFi network, synchronize time via NTP (Network Time Protocol),
 *   and display current time/date on the 1.8" TFT LCD display with real-time updates.
 *
 * HARDWARE:
 *   - Microcontroller: ESP32 or ESP32-C3
 *   - Display: 1.8" TFT LCD (128x160 pixels) with ST7735s controller
 *   - WiFi: Built-in WiFi module (all ESP32 variants)
 *
 * REQUIRED LIBRARIES:
 *   - Adafruit_ST7735 (https://github.com/adafruit/Adafruit-ST7735-Library)
 *   - Adafruit_GFX (https://github.com/adafruit/Adafruit-GFX-Library)
 *   - WiFi.h (Built-in ESP32 library)
 *   - time.h (Built-in C library for time functions)
 *
 * NETWORK CONFIGURATION:
 *   Multiple WiFi networks can be defined in the WIFI_NETWORKS array.
 *   The code will attempt to connect to each network in order until successful.
 *   If no networks are available, the display will show error and retry.
 *
 * NTP TIME SYNCHRONIZATION:
 *   - Uses pool.ntp.org NTP server (default)
 *   - Configurable timezone offset (currently UTC+0)
 *   - Automatic daylight saving adjustment if enabled
 *
 * ============================================================================
 */

#include <Adafruit_ST7735.h>
#include <Adafruit_GFX.h>
#include <SPI.h>
#include <WiFi.h>
#include <time.h>

// ============================================================================
// CUSTOM COLOR DEFINITIONS (Fixed RGB565 Color Order)
// ============================================================================

#define COLOR_BLACK   0x0000
#define COLOR_WHITE   0xFFFF
#define COLOR_RED     0xF800
#define COLOR_BLUE    0x001F
#define COLOR_GREEN   0x07E0
#define COLOR_CYAN    0x07FF
#define COLOR_YELLOW  0xFFE0
#define COLOR_MAGENTA 0xF81F

// ============================================================================
// WIFI CONFIGURATION - NETWORK ARRAY
// ============================================================================
// Define multiple WiFi networks. The system will try each in order.
// Add your networks here in SSID, Password pairs.

struct WiFiNetwork {
  const char* ssid;
  const char* password;
};

// Array of WiFi networks to try (in order of preference)
const WiFiNetwork WIFI_NETWORKS[] = {
  {"Itzz_Infinity", "123456789"},  // Primary network
  // Add additional networks below (uncomment and fill in your networks)
  // {"Network_Name_2", "Password_2"},
  // {"Network_Name_3", "Password_3"},
};

const int NUM_NETWORKS = sizeof(WIFI_NETWORKS) / sizeof(WiFiNetwork);

// ============================================================================
// NTP TIME CONFIGURATION
// ============================================================================

#define NTP_SERVER     "pool.ntp.org"
#define GMT_OFFSET_SEC 0              // UTC+0 (change to 3600 for UTC+1, etc.)
#define DST_OFFSET_SEC 0              // Daylight saving offset (usually 3600)

// ============================================================================
// PIN DEFINITIONS (Board-specific)
// ============================================================================

#if defined(ESP32) && !defined(CONFIG_IDF_TARGET_ESP32C3)
  // ESP32 (38-pin variant)
  #define TFT_CS   5
  #define TFT_RST  4
  #define TFT_DC   2
  #define TFT_MOSI 23
  #define TFT_SCK  18

#elif defined(CONFIG_IDF_TARGET_ESP32C3) || defined(ESP32C3)
  // ESP32-C3
  #define TFT_CS   7
  #define TFT_RST  8
  #define TFT_DC   10
  #define TFT_MOSI 6
  #define TFT_SCK  4

#else
  #error "Unsupported microcontroller. Please use ESP32 or ESP32-C3."
#endif

// ============================================================================
// GLOBAL VARIABLES & OBJECTS
// ============================================================================

Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_MOSI, TFT_SCK, TFT_RST);

// WiFi and time state
bool wifi_connected = false;
int current_network_index = -1;
unsigned long last_display_update = 0;
unsigned long last_wifi_check = 0;

// Display constants
#define DISPLAY_UPDATE_INTERVAL 1000  // Update display every 1 second
#define WIFI_CHECK_INTERVAL 10000     // Check WiFi connection every 10 seconds

// ============================================================================
// SETUP - Initialize hardware and WiFi
// ============================================================================

void setup() {
  Serial.begin(115200);
  delay(2000);
  
  // Log startup information
  Serial.println("\n\n===========================================");
  Serial.println("espFrameBuffer - Task 2: Internet Time");
  Serial.println("===========================================");
  
  #if defined(ESP32) && !defined(CONFIG_IDF_TARGET_ESP32C3)
    Serial.println("Board: ESP32");
  #elif defined(CONFIG_IDF_TARGET_ESP32C3) || defined(ESP32C3)
    Serial.println("Board: ESP32-C3");
  #endif
  
  Serial.print("WiFi Networks Available: ");
  Serial.println(NUM_NETWORKS);
  for (int i = 0; i < NUM_NETWORKS; i++) {
    Serial.print("  [");
    Serial.print(i + 1);
    Serial.print("] ");
    Serial.println(WIFI_NETWORKS[i].ssid);
  }
  
  // Initialize display
  Serial.println("Initializing TFT display...");
  tft.initR(INITR_BLACKTAB);
  tft.setRotation(0);
  tft.fillScreen(COLOR_BLACK);
  Serial.println("✓ Display initialized");
  
  // Display initial splash
  displayInitScreen();
  
  // Start WiFi connection
  Serial.println("Starting WiFi connection...");
  connectToWiFi();
  
  Serial.println("===========================================\n");
}

// ============================================================================
// MAIN LOOP
// ============================================================================

void loop() {
  unsigned long current_millis = millis();
  
  // Check WiFi connection status periodically
  if (current_millis - last_wifi_check >= WIFI_CHECK_INTERVAL) {
    last_wifi_check = current_millis;
    
    if (WiFi.status() != WL_CONNECTED) {
      Serial.println("WiFi disconnected, attempting reconnection...");
      wifi_connected = false;
      connectToWiFi();
    } else {
      wifi_connected = true;
    }
  }
  
  // Update display every second
  if (current_millis - last_display_update >= DISPLAY_UPDATE_INTERVAL) {
    last_display_update = current_millis;
    displayTimeAndDate();
  }
  
  delay(100);  // Small delay to prevent watchdog reset
}

// ============================================================================
// WIFI FUNCTIONS
// ============================================================================

/*
 * connectToWiFi()
 * Attempts to connect to available WiFi networks from the WIFI_NETWORKS array.
 * Tries each network in sequence until successful or all networks exhausted.
 */
void connectToWiFi() {
  Serial.println("\n--- Starting WiFi Connection Sequence ---");
  
  WiFi.mode(WIFI_STA);
  WiFi.disconnect(true);  // Turn off WiFi before scanning
  delay(100);
  
  // Scan for available networks
  Serial.println("Scanning available networks...");
  int networks_found = WiFi.scanNetworks();
  
  if (networks_found == 0) {
    Serial.println("No WiFi networks found!");
    displayError("No Networks\nFound");
    return;
  }
  
  Serial.print("Networks found: ");
  Serial.println(networks_found);
  
  // Try to connect to each network in WIFI_NETWORKS array
  for (int i = 0; i < NUM_NETWORKS; i++) {
    bool network_available = false;
    
    // Check if current network is in scan results
    for (int j = 0; j < networks_found; j++) {
      if (WiFi.SSID(j) == WIFI_NETWORKS[i].ssid) {
        network_available = true;
        break;
      }
    }
    
    if (!network_available) {
      Serial.print("Network not available: ");
      Serial.println(WIFI_NETWORKS[i].ssid);
      continue;
    }
    
    Serial.print("Attempting connection to: ");
    Serial.println(WIFI_NETWORKS[i].ssid);
    
    displayConnecting(WIFI_NETWORKS[i].ssid);
    
    // Attempt connection with timeout
    WiFi.begin(WIFI_NETWORKS[i].ssid, WIFI_NETWORKS[i].password);
    
    int timeout = 20;  // 20 attempts of 500ms = 10 seconds
    while (WiFi.status() != WL_CONNECTED && timeout > 0) {
      delay(500);
      Serial.print(".");
      timeout--;
    }
    
    if (WiFi.status() == WL_CONNECTED) {
      current_network_index = i;
      wifi_connected = true;
      
      Serial.println();
      Serial.print("✓ Connected! IP: ");
      Serial.println(WiFi.localIP());
      
      // Synchronize time via NTP
      Serial.println("Synchronizing time via NTP...");
      syncTimeNTP();
      
      displayConnected();
      return;
    }
    
    Serial.println("✗ Connection failed");
  }
  
  // If we reach here, no networks connected
  Serial.println("✗ Failed to connect to any network");
  displayError("Connection\nFailed");
  wifi_connected = false;
}

/*
 * syncTimeNTP()
 * Synchronizes system time using NTP server.
 * Should be called after successful WiFi connection.
 */
void syncTimeNTP() {
  // Configure time with NTP server
  configTime(GMT_OFFSET_SEC, DST_OFFSET_SEC, NTP_SERVER);
  
  // Wait for time to be set
  Serial.print("Waiting for NTP time sync... ");
  time_t now = time(nullptr);
  int timeout = 20;
  
  while (now < 24 * 3600 && timeout > 0) {
    delay(500);
    Serial.print(".");
    now = time(nullptr);
    timeout--;
  }
  
  Serial.println();
  
  if (now > 24 * 3600) {
    Serial.print("✓ Time synchronized: ");
    Serial.println(ctime(&now));
  } else {
    Serial.println("✗ NTP sync failed, using default time");
  }
}

// ============================================================================
// DISPLAY FUNCTIONS
// ============================================================================

/*
 * displayInitScreen()
 * Shows initial splash screen during startup.
 */
void displayInitScreen() {
  tft.fillScreen(COLOR_BLACK);
  
  tft.setCursor(10, 20);
  tft.setTextColor(COLOR_CYAN);
  tft.setTextSize(2);
  tft.println("Task 2");
  
  tft.setCursor(10, 50);
  tft.setTextColor(COLOR_WHITE);
  tft.setTextSize(1);
  tft.println("Internet Time");
  tft.println("Display");
  
  tft.drawLine(5, 75, 123, 75, COLOR_CYAN);
  
  tft.setCursor(10, 90);
  tft.setTextColor(COLOR_YELLOW);
  tft.setTextSize(1);
  tft.println("Initializing...");
  
  tft.setCursor(10, 140);
  tft.setTextColor(COLOR_MAGENTA);
  tft.setTextSize(1);
  tft.println("WiFi + NTP");
}

/*
 * displayConnecting(const char* ssid)
 * Shows connecting status with network name.
 */
void displayConnecting(const char* ssid) {
  tft.fillScreen(COLOR_BLACK);
  
  tft.setCursor(10, 20);
  tft.setTextColor(COLOR_YELLOW);
  tft.setTextSize(1);
  tft.println("Connecting to:");
  
  tft.setCursor(10, 40);
  tft.setTextColor(COLOR_CYAN);
  tft.setTextSize(1);
  tft.println(ssid);
  
  tft.drawLine(5, 60, 123, 60, COLOR_YELLOW);
  
  tft.setCursor(10, 80);
  tft.setTextColor(COLOR_WHITE);
  tft.println("Please wait...");
  
  // Animate dots
  for (int i = 0; i < 3; i++) {
    tft.println(".");
    delay(300);
  }
}

/*
 * displayConnected()
 * Shows connection successful message.
 */
void displayConnected() {
  tft.fillScreen(COLOR_BLACK);
  
  tft.setCursor(10, 20);
  tft.setTextColor(COLOR_GREEN);
  tft.setTextSize(1);
  tft.println("Connected!");
  
  tft.setCursor(10, 40);
  tft.setTextColor(COLOR_CYAN);
  tft.println(WIFI_NETWORKS[current_network_index].ssid);
  
  tft.setCursor(10, 60);
  tft.setTextColor(COLOR_WHITE);
  tft.print("IP: ");
  tft.println(WiFi.localIP());
  
  tft.drawLine(5, 80, 123, 80, COLOR_GREEN);
  
  tft.setCursor(10, 100);
  tft.setTextColor(COLOR_YELLOW);
  tft.println("Syncing time...");
  
  delay(2000);
}

/*
 * displayError(const char* error_msg)
 * Shows error message on display.
 */
void displayError(const char* error_msg) {
  tft.fillScreen(COLOR_BLACK);
  
  tft.setCursor(10, 30);
  tft.setTextColor(COLOR_RED);
  tft.setTextSize(1);
  tft.println("ERROR");
  
  tft.drawLine(5, 50, 123, 50, COLOR_RED);
  
  tft.setCursor(10, 70);
  tft.setTextColor(COLOR_WHITE);
  tft.setTextSize(1);
  tft.println(error_msg);
  
  tft.setCursor(10, 140);
  tft.setTextColor(COLOR_YELLOW);
  tft.print("Retry in 10s...");
}

/*
 * displayTimeAndDate()
 * Main display function showing current time, date, and WiFi status.
 * Called frequently to update display with fresh time data.
 */
void displayTimeAndDate() {
  tft.fillScreen(COLOR_BLACK);
  
  // Get current time
  time_t now = time(nullptr);
  struct tm* timeinfo = localtime(&now);
  
  // Draw header
  tft.setCursor(5, 5);
  tft.setTextColor(COLOR_CYAN);
  tft.setTextSize(1);
  tft.println("INTERNET TIME");
  
  // Draw WiFi status indicator
  int wifi_x = 100;
  int wifi_y = 5;
  if (wifi_connected && WiFi.status() == WL_CONNECTED) {
    tft.setTextColor(COLOR_GREEN);
    tft.setCursor(wifi_x, wifi_y);
    tft.println("WiFi");
  } else {
    tft.setTextColor(COLOR_RED);
    tft.setCursor(wifi_x - 10, wifi_y);
    tft.println("X NO WiFi");
  }
  
  // Draw separator line
  tft.drawLine(5, 20, 123, 20, COLOR_CYAN);
  
  // Draw large time display
  tft.setCursor(10, 40);
  tft.setTextColor(COLOR_YELLOW);
  tft.setTextSize(3);  // Large font for hours and minutes
  
  char time_buffer[3];
  sprintf(time_buffer, "%02d", timeinfo->tm_hour);
  tft.print(time_buffer);
  tft.print(":");
  sprintf(time_buffer, "%02d", timeinfo->tm_min);
  tft.print(time_buffer);
  tft.print(":");
  sprintf(time_buffer, "%02d", timeinfo->tm_sec);
  tft.print(time_buffer);
  
  // Draw seconds in smaller font
  tft.setTextSize(1);
  
  // Draw date
  tft.setCursor(10, 80);
  tft.setTextColor(COLOR_WHITE);
  tft.setTextSize(1);
  
  const char* day_names[] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};
  const char* month_names[] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", 
                               "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
  
  tft.print(day_names[timeinfo->tm_wday]);
  tft.print(" ");
  tft.print(month_names[timeinfo->tm_mon]);
  tft.print(" ");
  tft.print(timeinfo->tm_mday);
  tft.print(" ");
  tft.print(1900 + timeinfo->tm_year);
  
  // Draw network info
  tft.drawLine(5, 110, 123, 110, COLOR_MAGENTA);
  
  tft.setCursor(5, 120);
  tft.setTextColor(COLOR_MAGENTA);
  tft.setTextSize(1);
  
  if (wifi_connected && WiFi.status() == WL_CONNECTED) {
    tft.print("Network: ");
    tft.println(WIFI_NETWORKS[current_network_index].ssid);
    
    tft.print("IP: ");
    tft.println(WiFi.localIP());
    
    tft.print("Signal: ");
    int rssi = WiFi.RSSI();
    tft.print(rssi);
    tft.println(" dBm");
  } else {
    tft.println("Disconnected");
    tft.println("Attempting to connect...");
  }
}

// ============================================================================
// END OF FILE
// ============================================================================
