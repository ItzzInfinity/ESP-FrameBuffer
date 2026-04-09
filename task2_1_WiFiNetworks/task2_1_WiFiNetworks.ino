/*
 * ============================================================================
 * PROJECT: espFrameBuffer - Task 2.1: WiFi Networks & Status Display
 * ============================================================================
 * PURPOSE:
 *   Enhance Task 2 with WiFi network scanning, connection status display,
 *   and available WiFi networks list. Uses PARTIAL SCREEN UPDATES for
 *   efficiency - only updates changed regions to reduce flicker.
 *
 * KEY FEATURE: PARTIAL UPDATES
 *   Instead of refreshing entire 128x160 display, only updates the sections
 *   that change (connection status, network list, signal strength, etc.)
 *   This reduces CPU load, flicker, and power consumption.
 *
 * HARDWARE:
 *   - Microcontroller: ESP32 or ESP32-C3
 *   - Display: 1.8" TFT LCD (128x160 pixels) with ST7735s controller
 *   - WiFi: Built-in WiFi module
 *
 * REQUIRED LIBRARIES:
 *   - Adafruit_ST7735
 *   - Adafruit_GFX
 *   - WiFi.h (Built-in)
 *   - time.h (Built-in)
 *
 * SCREEN LAYOUT (128x160):
 *   ┌──────────────────────────┐
 *   │ WIFI STATUS (0-20px)      │  ← Connected/Disconnected indicator
 *   ├──────────────────────────┤
 *   │ AVAILABLE NETWORKS       │  ← List of WiFi networks (20-110px)
 *   │ Network1 [###]           │    Scrollable if >4 networks
 *   │ Network2 [##]            │
 *   │ Network3 [#]             │
 *   │ Network4                 │
 *   ├──────────────────────────┤
 *   │ TIME INFO (110-160px)    │  ← Current time & date
 *   │ 14:32:45                 │
 *   │ Mon 17 Apr               │
 *   └──────────────────────────┘
 *
 * ============================================================================
 */

#include <Adafruit_ST7735.h>
#include <Adafruit_GFX.h>
#include <SPI.h>
#include <WiFi.h>
#include <time.h>

// ============================================================================
// CUSTOM COLOR DEFINITIONS
// ============================================================================

#define COLOR_BLACK   0x0000
#define COLOR_WHITE   0xFFFF
#define COLOR_RED     0xF800
#define COLOR_BLUE    0x001F
#define COLOR_GREEN   0x07E0
#define COLOR_CYAN    0x07FF
#define COLOR_YELLOW  0xFFE0
#define COLOR_MAGENTA 0xF81F
#define COLOR_GRAY    0x8410  // Dark gray for inactive items

// ============================================================================
// WIFI NETWORK CONFIGURATION
// ============================================================================

struct WiFiNetwork {
  const char* ssid;
  const char* password;
};

const WiFiNetwork WIFI_NETWORKS[] = {
  {"Itzz_Infinity", "123456789"},
};

const int NUM_NETWORKS = sizeof(WIFI_NETWORKS) / sizeof(WiFiNetwork);

// ============================================================================
// NTP TIME CONFIGURATION
// ============================================================================

#define NTP_SERVER     "pool.ntp.org"
#define GMT_OFFSET_SEC 0
#define DST_OFFSET_SEC 0

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
#else
  #error "Unsupported microcontroller."
#endif

// ============================================================================
// SCREEN LAYOUT DEFINITIONS (Partial Update Regions)
// ============================================================================

// Region 1: WiFi Status Bar (Top)
#define STATUS_REGION_X 0
#define STATUS_REGION_Y 0
#define STATUS_REGION_W 128
#define STATUS_REGION_H 20

// Region 2: WiFi Networks List
#define NETWORKS_REGION_X 0
#define NETWORKS_REGION_Y 20
#define NETWORKS_REGION_W 128
#define NETWORKS_REGION_H 90

// Region 3: Time Display (Bottom)
#define TIME_REGION_X 0
#define TIME_REGION_Y 110
#define TIME_REGION_W 128
#define TIME_REGION_H 50

// ============================================================================
// NETWORK INFO STRUCTURE
// ============================================================================

struct WiFiNetworkInfo {
  const char* ssid;
  int rssi;
  int channel;
  bool is_open;
};

// ============================================================================
// GLOBAL VARIABLES
// ============================================================================

Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_MOSI, TFT_SCK, TFT_RST);

// WiFi state
bool wifi_connected = false;
int current_network_index = -1;
int num_networks_found = 0;
WiFiNetworkInfo available_networks[20];  // Store up to 20 networks

// Display state tracking (for efficient partial updates)
int last_network_count = -1;
bool last_wifi_state = false;
unsigned long last_time_update = 0;
unsigned long last_network_scan = 0;
unsigned long last_status_update = 0;
unsigned long last_display_update = 0;

// Timing
#define STATUS_UPDATE_INTERVAL 500    // Update status every 500ms
#define NETWORK_SCAN_INTERVAL 10000   // Scan networks every 10s
#define TIME_UPDATE_INTERVAL 1000     // Update time every 1s
#define DISPLAY_REDRAW_INTERVAL 100   // Min interval between updates

// Network list scrolling
int network_scroll_offset = 0;
#define NETWORKS_PER_PAGE 4
int total_network_pages = 1;

// ============================================================================
// SETUP
// ============================================================================

void setup() {
  Serial.begin(115200);
  delay(2000);
  
  Serial.println("\n\n===========================================");
  Serial.println("espFrameBuffer - Task 2.1: WiFi Networks");
  Serial.println("===========================================");
  
  #if defined(ESP32) && !defined(CONFIG_IDF_TARGET_ESP32C3)
    Serial.println("Board: ESP32");
  #elif defined(CONFIG_IDF_TARGET_ESP32C3) || defined(ESP32C3)
    Serial.println("Board: ESP32-C3");
  #endif
  
  // Initialize display
  Serial.println("Initializing TFT display...");
  tft.initR(INITR_BLACKTAB);
  tft.setRotation(0);
  tft.fillScreen(COLOR_BLACK);
  Serial.println("✓ Display initialized");
  
  // Draw initial layout
  displayInitialLayout();
  
  // Start WiFi connection
  Serial.println("Starting WiFi connection...");
  connectToWiFi();
  
  Serial.println("===========================================\n");
}

// ============================================================================
// MAIN LOOP - Efficient Partial Updates
// ============================================================================

void loop() {
  unsigned long current_millis = millis();
  
  // Scan networks periodically
  if (current_millis - last_network_scan >= NETWORK_SCAN_INTERVAL) {
    last_network_scan = current_millis;
    scanWiFiNetworks();
    updateNetworksDisplay();  // Partial update: networks region only
  }
  
  // Update WiFi connection status
  if (current_millis - last_status_update >= STATUS_UPDATE_INTERVAL) {
    last_status_update = current_millis;
    
    if (WiFi.status() != WL_CONNECTED) {
      if (wifi_connected) {
        wifi_connected = false;
        updateStatusDisplay();  // Partial update: status region only
        connectToWiFi();
      }
    } else if (!wifi_connected) {
      wifi_connected = true;
      updateStatusDisplay();    // Partial update: status region only
    }
  }
  
  // Update time display
  if (current_millis - last_time_update >= TIME_UPDATE_INTERVAL) {
    last_time_update = current_millis;
    updateTimeDisplay();  // Partial update: time region only
  }
  
  delay(50);  // Prevent watchdog reset
}

// ============================================================================
// DISPLAY FUNCTIONS - PARTIAL UPDATES
// ============================================================================

/*
 * displayInitialLayout()
 * Draws the static frame/borders on display once
 */
void displayInitialLayout() {
  tft.fillScreen(COLOR_BLACK);
  
  // Draw separator line between regions
  tft.drawLine(0, 20, 128, 20, COLOR_CYAN);
  tft.drawLine(0, 110, 128, 110, COLOR_CYAN);
  
  // Draw corner indicators
  tft.drawRect(2, 2, 3, 3, COLOR_CYAN);
  tft.drawRect(123, 2, 3, 3, COLOR_CYAN);
}

/*
 * updateStatusDisplay()
 * PARTIAL UPDATE: Only updates WiFi status region (top 20px)
 */
void updateStatusDisplay() {
  // Clear status region only
  tft.fillRect(STATUS_REGION_X, STATUS_REGION_Y, STATUS_REGION_W, STATUS_REGION_H, COLOR_BLACK);
  
  // Redraw separator
  tft.drawLine(0, 20, 128, 20, COLOR_CYAN);
  
  // Draw status indicator
  tft.setCursor(5, 5);
  tft.setTextSize(1);
  
  if (wifi_connected && WiFi.status() == WL_CONNECTED) {
    // Connected status
    tft.setTextColor(COLOR_GREEN);
    tft.print("CONNECTED");
    
    // Draw signal strength bars
    int rssi = WiFi.RSSI();
    int signal_bars = getSignalBars(rssi);
    
    tft.setCursor(90, 5);
    for (int i = 0; i < 4; i++) {
      if (i < signal_bars) {
        tft.print("[");
        tft.setTextColor(COLOR_GREEN);
        tft.print("#");
        tft.setTextColor(COLOR_GREEN);
        tft.print("]");
      } else {
        tft.setTextColor(COLOR_GRAY);
        tft.print("[ ]");
      }
    }
    
    // Show current network name and IP
    tft.setCursor(5, 12);
    tft.setTextColor(COLOR_CYAN);
    tft.print(WIFI_NETWORKS[current_network_index].ssid);
    
  } else {
    // Disconnected status
    tft.setTextColor(COLOR_RED);
    tft.print("DISCONNECTED");
    
    tft.setCursor(5, 12);
    tft.setTextColor(COLOR_YELLOW);
    tft.print("Scanning for networks...");
  }
}

/*
 * updateNetworksDisplay()
 * PARTIAL UPDATE: Only updates WiFi networks region (middle section)
 */
void updateNetworksDisplay() {
  // Clear networks region only
  tft.fillRect(NETWORKS_REGION_X, NETWORKS_REGION_Y, NETWORKS_REGION_W, NETWORKS_REGION_H, COLOR_BLACK);
  
  // Redraw separator
  tft.drawLine(0, 20, 128, 20, COLOR_CYAN);
  tft.drawLine(0, 110, 128, 110, COLOR_CYAN);
  
  // Draw header
  tft.setCursor(5, 25);
  tft.setTextColor(COLOR_CYAN);
  tft.setTextSize(1);
  tft.print("Available Networks: ");
  tft.println(num_networks_found);
  
  if (num_networks_found == 0) {
    tft.setCursor(10, 45);
    tft.setTextColor(COLOR_YELLOW);
    tft.println("No networks found");
    return;
  }
  
  // Calculate pagination
  total_network_pages = (num_networks_found + NETWORKS_PER_PAGE - 1) / NETWORKS_PER_PAGE;
  
  // Draw networks list (max NETWORKS_PER_PAGE visible)
  int start_index = network_scroll_offset * NETWORKS_PER_PAGE;
  int end_index = min(start_index + NETWORKS_PER_PAGE, num_networks_found);
  
  int y_pos = 40;
  for (int i = start_index; i < end_index; i++) {
    // Network name
    tft.setCursor(5, y_pos);
    tft.setTextColor(COLOR_WHITE);
    tft.setTextSize(1);
    tft.print(available_networks[i].ssid);
    
    // Signal strength bars
    int rssi = available_networks[i].rssi;
    int signal_bars = getSignalBars(rssi);
    
    tft.setCursor(90, y_pos);
    for (int j = 0; j < signal_bars; j++) {
      tft.setTextColor(COLOR_GREEN);
      tft.print("#");
    }
    for (int j = signal_bars; j < 4; j++) {
      tft.setTextColor(COLOR_GRAY);
      tft.print("-");
    }
    
    // RSSI value
    tft.setCursor(115, y_pos);
    tft.setTextColor(COLOR_MAGENTA);
    tft.print(rssi);
    
    y_pos += 16;
  }
  
  // Draw pagination indicator
  if (total_network_pages > 1) {
    tft.setCursor(50, 105);
    tft.setTextColor(COLOR_YELLOW);
    tft.setTextSize(1);
    tft.print("Page ");
    tft.print(network_scroll_offset + 1);
    tft.print("/");
    tft.print(total_network_pages);
  }
}

/*
 * updateTimeDisplay()
 * PARTIAL UPDATE: Only updates time region (bottom section)
 */
void updateTimeDisplay() {
  // Clear time region only
  tft.fillRect(TIME_REGION_X, TIME_REGION_Y, TIME_REGION_W, TIME_REGION_H, COLOR_BLACK);
  
  // Redraw separator
  tft.drawLine(0, 110, 128, 110, COLOR_CYAN);
  
  // Get current time
  time_t now = time(nullptr);
  struct tm* timeinfo = localtime(&now);
  
  // Draw large time
  tft.setCursor(15, 115);
  tft.setTextColor(COLOR_YELLOW);
  tft.setTextSize(2);
  
  char time_buffer[9];
  sprintf(time_buffer, "%02d:%02d:%02d", 
          timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec);
  tft.println(time_buffer);
  
  // Draw date
  tft.setCursor(10, 140);
  tft.setTextColor(COLOR_WHITE);
  tft.setTextSize(1);
  
  const char* day_names[] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};
  const char* month_names[] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", 
                               "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
  
  sprintf(time_buffer, "%s %d %s", 
          day_names[timeinfo->tm_wday], 
          timeinfo->tm_mday, 
          month_names[timeinfo->tm_mon]);
  tft.print(time_buffer);
}

// ============================================================================
// HELPER FUNCTIONS
// ============================================================================

/*
 * getSignalBars(int rssi)
 * Converts RSSI (Received Signal Strength Indicator) to 0-4 bars
 * RSSI ranges from -100 (weak) to -30 (strong) dBm
 */
int getSignalBars(int rssi) {
  if (rssi >= -50) return 4;      // Excellent
  if (rssi >= -60) return 3;      // Good
  if (rssi >= -70) return 2;      // Fair
  if (rssi >= -80) return 1;      // Weak
  return 0;                        // No signal
}

/*
 * scanWiFiNetworks()
 * Scans for available WiFi networks and stores in array
 * Called periodically to update network list
 */
void scanWiFiNetworks() {
  Serial.println("Scanning for WiFi networks...");
  
  // Perform WiFi scan
  int n = WiFi.scanNetworks();
  
  if (n == 0) {
    Serial.println("No networks found");
    num_networks_found = 0;
    return;
  }
  
  // Store networks (max 20)
  num_networks_found = min(n, 20);
  
  for (int i = 0; i < num_networks_found; i++) {
    available_networks[i].ssid = WiFi.SSID(i);
    available_networks[i].rssi = WiFi.RSSI(i);
    available_networks[i].channel = WiFi.channel(i);
    available_networks[i].is_open = WiFi.encryptionType(i) == WIFI_AUTH_OPEN;
    
    Serial.print("  [");
    Serial.print(i + 1);
    Serial.print("] ");
    Serial.print(available_networks[i].ssid);
    Serial.print(" (");
    Serial.print(available_networks[i].rssi);
    Serial.println(" dBm)");
  }
}

/*
 * connectToWiFi()
 * Connects to first available network from WIFI_NETWORKS array
 */
void connectToWiFi() {
  Serial.println("Starting WiFi connection sequence...");
  
  WiFi.mode(WIFI_STA);
  WiFi.disconnect(true);
  delay(100);
  
  // Scan for networks
  scanWiFiNetworks();
  updateNetworksDisplay();  // Show found networks
  
  if (num_networks_found == 0) {
    Serial.println("No networks available");
    updateStatusDisplay();
    return;
  }
  
  // Try to connect to each configured network
  for (int i = 0; i < NUM_NETWORKS; i++) {
    bool found = false;
    
    for (int j = 0; j < num_networks_found; j++) {
      if (available_networks[j].ssid == WIFI_NETWORKS[i].ssid) {
        found = true;
        break;
      }
    }
    
    if (!found) {
      Serial.print("Network not available: ");
      Serial.println(WIFI_NETWORKS[i].ssid);
      continue;
    }
    
    Serial.print("Connecting to: ");
    Serial.println(WIFI_NETWORKS[i].ssid);
    updateStatusDisplay();  // Show connecting status
    
    WiFi.begin(WIFI_NETWORKS[i].ssid, WIFI_NETWORKS[i].password);
    
    int timeout = 20;
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
      
      // Sync time
      Serial.println("Synchronizing time with NTP...");
      configTime(GMT_OFFSET_SEC, DST_OFFSET_SEC, NTP_SERVER);
      
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
      }
      
      updateStatusDisplay();
      return;
    }
    
    Serial.println("Connection failed");
  }
  
  Serial.println("Could not connect to any network");
  wifi_connected = false;
  updateStatusDisplay();
}

// ============================================================================
// END OF FILE
// ============================================================================
