/*
 * ============================================================================
 * PROJECT: espFrameBuffer - Task 2.1 (CLEAN VERSION)
 * ============================================================================
 */

#include <Adafruit_ST7735.h>
#include <Adafruit_GFX.h>
#include <SPI.h>
#include <WiFi.h>

// ============================================================================
// COLORS
// ============================================================================
#define COLOR_BLACK    0x0000
#define COLOR_WHITE    0xFFFF
#define COLOR_GREEN    0x07E0
#define COLOR_CYAN     0x07FF
#define COLOR_YELLOW   0xFFE0
#define COLOR_MAGENTA  0xF81F
#define COLOR_GRAY     0x8410

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
// CONFIG
// ============================================================================
#define SCAN_INTERVAL      10000
#define NETWORKS_PER_PAGE  5
#define MAX_NETWORKS       20

// ============================================================================
// STRUCT
// ============================================================================
struct WiFiNetworkInfo {
  String ssid;
  int rssi;
  int channel;
};

// ============================================================================
// GLOBALS
// ============================================================================
Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_MOSI, TFT_SCK, TFT_RST);

WiFiNetworkInfo networks[MAX_NETWORKS];

int numNetworks = 0;
int currentPage = 0;
int totalPages = 1;

unsigned long lastScanTime = 0;

// ============================================================================
// SETUP
// ============================================================================
void setup() {
  Serial.begin(115200);
  delay(1000);

  // Display init
  tft.initR(INITR_BLACKTAB);
  tft.setRotation(0);
  tft.fillScreen(COLOR_BLACK);

  // WiFi init
  WiFi.mode(WIFI_STA);
  WiFi.disconnect(true);

  Serial.println("WiFi Scanner Started");

  scanNetworks();
}

// ============================================================================
// LOOP
// ============================================================================
void loop() {
  if (millis() - lastScanTime > SCAN_INTERVAL) {
    scanNetworks();
    lastScanTime = millis();
  }
}

// ============================================================================
// WIFI SCAN
// ============================================================================
void scanNetworks() {
  Serial.println("\nScanning...");

  int found = WiFi.scanNetworks();

  numNetworks = min(found, MAX_NETWORKS);

  for (int i = 0; i < numNetworks; i++) {
    networks[i].ssid    = WiFi.SSID(i);
    networks[i].rssi    = WiFi.RSSI(i);
    networks[i].channel = WiFi.channel(i);

    Serial.printf("[%d] %s (%d dBm)\n",
                  i + 1,
                  networks[i].ssid.c_str(),
                  networks[i].rssi);
  }

  totalPages = (numNetworks + NETWORKS_PER_PAGE - 1) / NETWORKS_PER_PAGE;
  currentPage = 0;

  drawUI();
}

// ============================================================================
// DISPLAY UI
// ============================================================================
void drawUI() {
  tft.fillScreen(COLOR_BLACK);

  drawHeader();
  drawNetworkList();
  drawFooter();
}

// --------------------------------------------------------------------------

void drawHeader() {
  tft.setCursor(5, 5);
  tft.setTextColor(COLOR_CYAN);
  tft.setTextSize(1);

  tft.print("WiFi: ");
  tft.println(numNetworks);

  tft.drawLine(0, 15, 128, 15, COLOR_CYAN);
}

// --------------------------------------------------------------------------

void drawNetworkList() {
  if (numNetworks == 0) {
    tft.setCursor(10, 40);
    tft.setTextColor(COLOR_YELLOW);
    tft.print("No Networks");
    return;
  }

  int start = currentPage * NETWORKS_PER_PAGE;
  int end   = min(start + NETWORKS_PER_PAGE, numNetworks);

  int y = 25;

  for (int i = start; i < end; i++) {
    drawSingleNetwork(i, y);
    y += 20;
  }
}

// --------------------------------------------------------------------------

void drawSingleNetwork(int i, int y) {
  // SSID (truncate)
  tft.setCursor(5, y);
  tft.setTextColor(COLOR_WHITE);

  String name = networks[i].ssid;
  if (name.length() > 14) {
    name = name.substring(0, 14);
  }
  tft.print(name);

  // Signal bars
  int bars = getSignalBars(networks[i].rssi);

  tft.setCursor(85, y);

  for (int j = 0; j < 4; j++) {
    if (j < bars) {
      tft.setTextColor(COLOR_GREEN);
      tft.print("#");
    } else {
      tft.setTextColor(COLOR_GRAY);
      tft.print("-");
    }
  }

  // RSSI
  tft.setCursor(105, y);
  tft.setTextColor(COLOR_MAGENTA);
  tft.print(networks[i].rssi);
}

// --------------------------------------------------------------------------

void drawFooter() {
  // Page info
  if (totalPages > 1) {
    tft.setCursor(40, 155);
    tft.setTextColor(COLOR_YELLOW);
    tft.print(currentPage + 1);
    tft.print("/");
    tft.print(totalPages);
  }

  // Status
  tft.setCursor(5, 155);
  tft.setTextColor(COLOR_CYAN);
  tft.print("Scanning...");
}

// ============================================================================
// SIGNAL STRENGTH
// ============================================================================
int getSignalBars(int rssi) {
  if (rssi >= -50) return 4;
  if (rssi >= -60) return 3;
  if (rssi >= -70) return 2;
  if (rssi >= -80) return 1;
  return 0;
}
// /*
//  * ============================================================================
//  * PROJECT: espFrameBuffer - Task 2.1: WiFi Network Scanner
//  * ============================================================================
//  * PURPOSE:
//  *   Simple WiFi network scanner that displays available networks on TFT LCD
//  *   with signal strength indicators. Scans every 10 seconds and updates display.
//  *
//  * HARDWARE:
//  *   - Microcontroller: ESP32 or ESP32-C3
//  *   - Display: 1.8" TFT LCD (128x160 pixels) with ST7735s controller
//  *   - WiFi: Built-in WiFi module
//  *
//  * REQUIRED LIBRARIES:
//  *   - Adafruit_ST7735
//  *   - Adafruit_GFX
//  *   - WiFi.h (Built-in)
//  *
//  * ============================================================================
//  */

// #include <Adafruit_ST7735.h>
// #include <Adafruit_GFX.h>
// #include <SPI.h>
// #include <WiFi.h>

// // ============================================================================
// // COLOR DEFINITIONS
// // ============================================================================

// #define COLOR_BLACK    0x0000
// #define COLOR_WHITE    0xFFFF
// #define COLOR_GREEN    0x07E0
// #define COLOR_CYAN     0x07FF
// #define COLOR_YELLOW   0xFFE0
// #define COLOR_MAGENTA  0xF81F
// #define COLOR_GRAY     0x8410

// // ============================================================================
// // PIN DEFINITIONS
// // ============================================================================

// #if defined(ESP32) && !defined(CONFIG_IDF_TARGET_ESP32C3)
//   #define TFT_CS   5
//   #define TFT_RST  4
//   #define TFT_DC   2
//   #define TFT_MOSI 23
//   #define TFT_SCK  18
// #elif defined(CONFIG_IDF_TARGET_ESP32C3) || defined(ESP32C3)
//   #define TFT_CS   7
//   #define TFT_RST  8
//   #define TFT_DC   10
//   #define TFT_MOSI 6
//   #define TFT_SCK  4
// #else
//   #error "Unsupported microcontroller."
// #endif

// // ============================================================================
// // DISPLAY & TIMING
// // ============================================================================

// #define NETWORK_SCAN_INTERVAL 10000  // Scan every 10 seconds
// #define NETWORKS_PER_PAGE 5          // Show 5 networks per screen
// #define MAX_NETWORKS 20              // Store max 20 networks

// // ============================================================================
// // GLOBAL VARIABLES
// // ============================================================================

// Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_MOSI, TFT_SCK, TFT_RST);

// struct WiFiNetworkInfo {
//   const char* ssid;
//   int rssi;
//   int channel;
// };

// WiFiNetworkInfo networks[MAX_NETWORKS];
// int num_networks_found = 0;
// unsigned long last_scan = 0;
// int network_scroll_offset = 0;
// int total_pages = 1;

// // ============================================================================
// // SETUP
// // ============================================================================

// void setup() {
//   Serial.begin(115200);
//   delay(2000);
  
//   Serial.println("\n\nWiFi Network Scanner - Starting...");
  
//   // Initialize display
//   tft.initR(INITR_BLACKTAB);
//   tft.setRotation(0);
//   tft.fillScreen(COLOR_BLACK);
  
//   // Initialize WiFi
//   WiFi.mode(WIFI_STA);
//   WiFi.disconnect(true);
  
//   Serial.println("✓ Display & WiFi initialized");
  
//   // Initial scan
//   scanNetworks();
// }

// // ============================================================================
// // MAIN LOOP
// // ============================================================================

// void loop() {
//   unsigned long current_millis = millis();
  
//   // Scan networks every 10 seconds
//   if (current_millis - last_scan >= NETWORK_SCAN_INTERVAL) {
//     last_scan = current_millis;
//     scanNetworks();
//   }
  
//   delay(100);
// }

// // ============================================================================
// // SCAN & DISPLAY FUNCTIONS
// // ============================================================================

// void scanNetworks() {
//   Serial.println("\nScanning for WiFi networks...");
  
//   int n = WiFi.scanNetworks();
  
//   if (n == 0) {
//     Serial.println("No networks found");
//     num_networks_found = 0;
//   } else {
//     num_networks_found = min(n, MAX_NETWORKS);
    
//     for (int i = 0; i < num_networks_found; i++) {
//       networks[i].ssid = WiFi.SSID(i);
//       networks[i].rssi = WiFi.RSSI(i);
//       networks[i].channel = WiFi.channel(i);
      
//       Serial.print("  [");
//       Serial.print(i + 1);
//       Serial.print("] ");
//       Serial.print(networks[i].ssid);
//       Serial.print(" (");
//       Serial.print(networks[i].rssi);
//       Serial.println(" dBm)");
//     }
//   }
  
//   // Calculate pages for pagination
//   total_pages = (num_networks_found + NETWORKS_PER_PAGE - 1) / NETWORKS_PER_PAGE;
//   network_scroll_offset = 0;
  
//   displayNetworks();
// }

// void displayNetworks() {
//   // Clear display
//   tft.fillScreen(COLOR_BLACK);
  
//   // Header
//   tft.setCursor(5, 5);
//   tft.setTextColor(COLOR_CYAN);
//   tft.setTextSize(1);
//   tft.print("WiFi Networks: ");
//   tft.println(num_networks_found);
  
//   // Draw line
//   tft.drawLine(0, 15, 128, 15, COLOR_CYAN);
  
//   if (num_networks_found == 0) {
//     tft.setCursor(10, 40);
//     tft.setTextColor(COLOR_YELLOW);
//     tft.println("No networks found");
//     return;
//   }
  
//   // Draw network list
//   int start_idx = network_scroll_offset * NETWORKS_PER_PAGE;
//   int end_idx = min(start_idx + NETWORKS_PER_PAGE, num_networks_found);
  
//   int y_pos = 25;
//   for (int i = start_idx; i < end_idx; i++) {
//     // Network name
//     tft.setCursor(5, y_pos);
//     tft.setTextColor(COLOR_WHITE);
//     tft.setTextSize(1);
    
//     // Truncate long SSIDs
//     char ssid_display[16];
//     strncpy(ssid_display, networks[i].ssid, 15);
//     ssid_display[15] = '\0';
//     tft.print(ssid_display);
    
//     // Signal bars
//     int signal_bars = getSignalBars(networks[i].rssi);
//     tft.setCursor(85, y_pos);
    
//     for (int j = 0; j < signal_bars; j++) {
//       tft.setTextColor(COLOR_GREEN);
//       tft.print("#");
//     }
//     for (int j = signal_bars; j < 4; j++) {
//       tft.setTextColor(COLOR_GRAY);
//       tft.print("-");
//     }
    
//     // RSSI value
//     tft.setCursor(105, y_pos);
//     tft.setTextColor(COLOR_MAGENTA);
//     tft.print(networks[i].rssi);
    
//     y_pos += 20;
//   }
  
//   // Pagination info
//   if (total_pages > 1) {
//     tft.setCursor(40, 155);
//     tft.setTextColor(COLOR_YELLOW);
//     tft.setTextSize(1);
//     tft.print("Page ");
//     tft.print(network_scroll_offset + 1);
//     tft.print("/");
//     tft.print(total_pages);
//   }
  
//   // Scan indicator
//   tft.setCursor(5, 155);
//   tft.setTextColor(COLOR_CYAN);
//   tft.setTextSize(1);
//   tft.print("Scanning...");
// }

// int getSignalBars(int rssi) {
//   if (rssi >= -50) return 4;   // Excellent
//   if (rssi >= -60) return 3;   // Good
//   if (rssi >= -70) return 2;   // Fair
//   if (rssi >= -80) return 1;   // Weak
//   return 0;                     // No signal
// }

// // ============================================================================
// // END OF FILE
// // ============================================================================
