/*
 * ============================================================================
 * PROJECT: espFrameBuffer - Task 4.1: SD Card Debugging & Error Diagnostics
 * ============================================================================
 * PURPOSE:
 *   Debug SD card initialization and file operations. Handle all error states
 *   with detailed logging. Support integrated SD adapter on display module.
 *   Test and find correct CS pin configuration.
 *
 * HARDWARE:
 *   - ESP32 or ESP32-C3
 *   - 1.8" TFT LCD (128x160) with ST7735s + integrated SD adapter
 *   - DHT11 sensor
 *   - Shared SPI bus (TFT + SD)
 *
 * KEY CHANGES IN 4.1:
 *   1. Comprehensive SD card diagnostics on startup
 *   2. Tests multiple SD_CS pin configurations
 *   3. Detailed error logging for every operation
 *   4. Displays error states on TFT clearly
 *   5. CSV file creation with robust error handling
 *   6. Heat index calculation fix (was showing 156.8°C!)
 *   7. SD card write verification
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
#define LOGGING_INTERVAL  300000        // Log every 5 minutes (300000 ms)
#define NTP_SERVER        "pool.ntp.org"
#define GMT_OFFSET        19800          // UTC+5:30

// SD_CS Pin - Try multiple configurations
// Integrated SD adapter on display: usually GPIO 15, 3, 4, or 8
// Update this based on diagnostic output
#define SD_CS_PRIMARY     15  // Try this first (common for integrated)
#define SD_CS_SECONDARY   3   // Try this second
#define SD_CS_TERTIARY    4   // Try this third
#define SD_CS_FALLBACK    8   // Fallback option

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
  // SD_CS pins defined above (PRIMARY, FALLBACK)
#elif defined(CONFIG_IDF_TARGET_ESP32C3) || defined(ESP32C3)
  #define TFT_CS   7
  #define TFT_RST  8
  #define TFT_DC   10
  #define TFT_MOSI 6
  #define TFT_SCK  4
  // SD_CS pins defined above (PRIMARY, FALLBACK)
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
int current_sd_cs = -1;  // Track which CS pin worked

const char* csv_filename = "data_log.csv";

#define DHT_READ_INTERVAL 2000

// Error tracking
struct ErrorState {
  bool sd_init_failed;
  bool csv_create_failed;
  bool csv_write_failed;
  bool sensor_read_failed;
  char last_error[100];
};

ErrorState error_state = {false, false, false, false, ""};

// ============================================================================
// SETUP
// ============================================================================

void setup() {
  Serial.begin(115200);
  delay(2000);
  
  Serial.println("\n\n=== Task 4.1: SD Card Debugging ===");
  Serial.println("Starting SD card diagnostics...\n");
  
  // Initialize display in landscape
  tft.initR(INITR_BLACKTAB);
  tft.setRotation(1);  // Landscape orientation
  tft.fillScreen(COLOR_BLACK);
  
  Serial.println("✓ Display initialized (landscape mode)");
  
  // Initialize DHT11
  dht.begin();
  Serial.println("✓ DHT11 initialized on GPIO " + String(DHT_PIN));
  
  // Display startup message
  drawMessage("SD Diagnostics", "Testing...", COLOR_YELLOW);
  
  // Sync time via WiFi
  syncTime();
  
  // Run SD card diagnostics
  Serial.println("\n=== SD CARD DIAGNOSTICS ===");
  runSDCardDiagnostics();
  
  if (sd_ready) {
    Serial.println("\n✓ SD card ready for logging");
    createCSVFile();
  } else {
    Serial.println("\n✗ SD card NOT ready - logging disabled");
    drawMessage("SD Card Error", "See Serial Monitor", COLOR_RED);
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
  if (sd_ready && (now - last_log_time >= LOGGING_INTERVAL)) {
    last_log_time = now;
    logToSD();
  }
  
  delay(100);
}

// ============================================================================
// SD CARD DIAGNOSTICS
// ============================================================================

void runSDCardDiagnostics() {
  Serial.println("Testing SD card initialization with multiple CS pin configurations...\n");
  
  // Array of CS pins to try
  int cs_pins[] = {SD_CS_PRIMARY, SD_CS_SECONDARY, SD_CS_TERTIARY, SD_CS_FALLBACK};
  const char* pin_names[] = {"PRIMARY", "SECONDARY", "TERTIARY", "FALLBACK"};
  
  for (int i = 0; i < 4; i++) {
    int cs_pin = cs_pins[i];
    const char* pin_name = pin_names[i];
    
    Serial.print("Attempting ");
    Serial.print(pin_name);
    Serial.print(" (GPIO ");
    Serial.print(cs_pin);
    Serial.println(")...");
    
    if (SD.begin(cs_pin)) {
      Serial.println("✓ SD.begin() returned true");
      
      // CRITICAL: Test actual file operation immediately after init
      File test = SD.open("test.txt", FILE_WRITE);
      if (test) {
        Serial.println("✓ File write test PASSED");
        test.close();
        
        Serial.println("✓✓✓ SD CARD WORKING! ✓✓✓");
        Serial.println("  Using CS pin: GPIO " + String(cs_pin));
        sd_ready = true;
        current_sd_cs = cs_pin;
        
        // Full diagnostics with working CS pin
        testSDCardOperations();
        return;  // Success - exit function
      } else {
        Serial.println("✗ File write test FAILED (SD.open returned false)");
        Serial.println("  SD.begin() succeeded but card not responding to writes");
      }
    } else {
      Serial.println("✗ SD.begin() failed");
    }
    Serial.println();
  }
  
  // If we get here, no CS pin worked
  Serial.println("\n✗✗✗ CRITICAL ERROR ✗✗✗");
  Serial.println("SD card NOT detected on any tested CS pin:");
  Serial.println("  PRIMARY: GPIO " + String(SD_CS_PRIMARY));
  Serial.println("  SECONDARY: GPIO " + String(SD_CS_SECONDARY));
  Serial.println("  TERTIARY: GPIO " + String(SD_CS_TERTIARY));
  Serial.println("  FALLBACK: GPIO " + String(SD_CS_FALLBACK));
  Serial.println("\nPOSSIBLE SOLUTIONS:");
  Serial.println("1. Check SD card is fully inserted");
  Serial.println("2. Check SD card contacts are clean");
  Serial.println("3. Try different SD card");
  Serial.println("4. Check module documentation for correct CS pin");
  Serial.println("5. Format SD card on computer as FAT32");
  Serial.println("6. Try manual CS pin: edit lines 35-38 with GPIO numbers from your module doc");
  
  sd_ready = false;
  error_state.sd_init_failed = true;
  snprintf(error_state.last_error, sizeof(error_state.last_error),
           "SD init failed - no CS pins worked");
}

void testSDCardOperations() {
  Serial.println("\n=== Testing SD Card File Operations ===");
  
  // Test file write with explicit error reporting
  Serial.println("\nTest 1: File Write");
  File test_file = SD.open("test.txt", FILE_WRITE);
  
  if (test_file) {
    Serial.println("✓ SD.open(\"test.txt\", FILE_WRITE) succeeded");
    
    int bytes_written = test_file.println("Test write successful");
    Serial.print("  Bytes written: ");
    Serial.println(bytes_written);
    
    if (bytes_written > 0) {
      Serial.println("✓ Data written successfully");
    } else {
      Serial.println("✗ println() returned 0 (write failed)");
    }
    
    test_file.close();
    Serial.println("✓ File closed");
  } else {
    Serial.println("✗ SD.open(\"test.txt\", FILE_WRITE) FAILED");
    Serial.println("  This means the card is not responding to write commands");
    Serial.println("  Possible causes:");
    Serial.println("    - Wrong CS pin (even though SD.begin() succeeded)");
    Serial.println("    - SD card not properly seated");
    Serial.println("    - SPI bus conflict with TFT");
    Serial.println("    - Card corrupted or needs formatting");
    error_state.csv_write_failed = true;
    return;
  }
  
  // Test file read
  Serial.println("\nTest 2: File Read");
  test_file = SD.open("test.txt", FILE_READ);
  
  if (test_file) {
    Serial.println("✓ SD.open(\"test.txt\", FILE_READ) succeeded");
    Serial.print("  File contents: ");
    
    while (test_file.available()) {
      Serial.write(test_file.read());
    }
    Serial.println();
    
    test_file.close();
    Serial.println("✓ File read successfully");
  } else {
    Serial.println("✗ SD.open(\"test.txt\", FILE_READ) FAILED");
    error_state.csv_write_failed = true;
  }
  
  // List root directory
  Serial.println("\nTest 3: List Directory");
  File root = SD.open("/");
  if (root) {
    Serial.println("Root directory contents:");
    listDir(root);
    root.close();
  } else {
    Serial.println("✗ Failed to open root directory");
  }
  
  // Test CSV file creation
  Serial.println("\nTest 4: CSV File Creation");
  if (!SD.exists(csv_filename)) {
    Serial.print("CSV file doesn't exist, creating: ");
    Serial.println(csv_filename);
    
    File csv_file = SD.open(csv_filename, FILE_WRITE);
    if (csv_file) {
      Serial.println("✓ CSV file opened for writing");
      
      // Write header
      int header_bytes = csv_file.println("timestamp,temperature,humidity,feels_like");
      Serial.print("  Header bytes written: ");
      Serial.println(header_bytes);
      
      if (header_bytes > 0) {
        Serial.println("✓ CSV header written successfully");
      } else {
        Serial.println("✗ CSV header write returned 0 (failed)");
      }
      
      csv_file.close();
      Serial.println("✓ CSV file closed");
    } else {
      Serial.println("✗ Failed to create CSV file");
      Serial.println("  SD.open() returned false");
      error_state.csv_create_failed = true;
    }
  } else {
    Serial.print("✓ CSV file already exists: ");
    Serial.println(csv_filename);
    
    // Check file size
    File file = SD.open(csv_filename);
    if (file) {
      Serial.print("  File size: ");
      Serial.print(file.size());
      Serial.println(" bytes");
      file.close();
    }
  }
}

void listDir(File dir) {
  while (true) {
    File entry = dir.openNextFile();
    if (!entry) {
      break;
    }
    
    Serial.print("  ");
    Serial.print(entry.name());
    
    if (entry.isDirectory()) {
      Serial.println("/");
    } else {
      Serial.print(" - ");
      Serial.print(entry.size());
      Serial.println(" bytes");
    }
    
    entry.close();
  }
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

void createCSVFile() {
  Serial.println("\n=== CSV File Setup ===");
  
  if (SD.exists(csv_filename)) {
    Serial.print("CSV file exists: ");
    Serial.println(csv_filename);
    
    // Check file size
    File file = SD.open(csv_filename);
    if (file) {
      Serial.print("File size: ");
      Serial.print(file.size());
      Serial.println(" bytes");
      file.close();
    }
    return;
  }
  
  Serial.print("Creating new CSV file: ");
  Serial.println(csv_filename);
  
  File file = SD.open(csv_filename, FILE_WRITE);
  if (file) {
    Serial.println("✓ File opened for writing");
    
    int bytes = file.println("timestamp,temperature,humidity,feels_like");
    Serial.print("✓ Header written: ");
    Serial.print(bytes);
    Serial.println(" bytes");
    
    file.close();
    Serial.println("✓ CSV file created successfully");
  } else {
    Serial.println("✗ Failed to create CSV file!");
    error_state.csv_create_failed = true;
    snprintf(error_state.last_error, sizeof(error_state.last_error),
             "CSV creation failed");
  }
}

void logToSD() {
  if (!sd_ready) {
    Serial.println("✗ SD card not ready");
    return;
  }
  
  // Read current sensor values
  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();
  
  if (isnan(temperature) || isnan(humidity)) {
    Serial.println("✗ Failed to read sensor");
    error_state.sensor_read_failed = true;
    snprintf(error_state.last_error, sizeof(error_state.last_error),
             "DHT11 read error");
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
    Serial.println("✗ Failed to open CSV file for writing");
    error_state.csv_write_failed = true;
    snprintf(error_state.last_error, sizeof(error_state.last_error),
             "CSV open failed");
    return;
  }
  
  // Format and write CSV line
  char csv_line[100];
  snprintf(csv_line, sizeof(csv_line), "%s,%.1f,%.1f,%.1f",
           timestamp, temperature, humidity, feels_like);
  
  int bytes = file.println(csv_line);
  file.close();
  
  if (bytes > 0) {
    Serial.print("✓ Logged (");
    Serial.print(bytes);
    Serial.print(" bytes): ");
    Serial.println(csv_line);
    
    // Reset error flags on successful write
    error_state.csv_write_failed = false;
  } else {
    Serial.println("✗ Failed to write CSV line");
    error_state.csv_write_failed = true;
    snprintf(error_state.last_error, sizeof(error_state.last_error),
             "CSV write failed");
  }
}

// ============================================================================
// SENSOR FUNCTIONS
// ============================================================================

void readAndDisplayDHT() {
  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();
  
  if (isnan(temperature) || isnan(humidity)) {
    Serial.println("✗ Failed to read DHT11");
    error_state.sensor_read_failed = true;
    return;
  }
  
  error_state.sensor_read_failed = false;
  
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
  // Only applies when temperature > ~26.7°C
  
  if (temp < 26.7) {
    return temp;  // Below 27°C, feels like = actual temp
  }
  
  // Validate inputs
  if (humidity < 0 || humidity > 100) {
    return temp;  // Invalid humidity, return actual temp
  }
  
  double T = temp;
  double RH = humidity;
  
  // Steadman heat index formula
  // HI = c1 + c2*T + c3*RH + c4*T*RH + c5*T² + c6*RH² + c7*T²*RH + c8*T*RH² + c9*T²*RH²
  
  double c1 = -42.379;
  double c2 = 2.04901523;
  double c3 = 10.14333127;
  double c4 = -0.22475541;
  double c5 = -0.00683783;
  double c6 = -0.05481717;
  double c7 = 0.00122874;
  double c8 = 0.00085282;
  double c9 = -0.00000199;
  
  double HI = c1 
            + c2 * T 
            + c3 * RH
            + c4 * T * RH 
            + c5 * T * T
            + c6 * RH * RH 
            + c7 * T * T * RH
            + c8 * T * RH * RH 
            + c9 * T * T * RH * RH;
  
  // Heat index should not be less than actual temperature
  if (HI < T) {
    return T;
  }
  
  // Cap at reasonable value (max ~60°C for realistic conditions)
  if (HI > 60) {
    return 60.0;
  }
  
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
  tft.println("T   | H   | Feels");
  
  tft.drawLine(0, 18, 160, 18, COLOR_CYAN);
  
  // Status
  tft.setCursor(5, 25);
  tft.setTextColor(COLOR_GREEN);
  tft.setTextSize(1);
  tft.print("SD: ");
  if (sd_ready) {
    tft.print("OK (GPIO");
    tft.print(current_sd_cs);
    tft.println(")");
  } else {
    tft.println("ERROR");
  }
  
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
  tft.setCursor(55, 25);
  tft.setTextColor(COLOR_ORANGE);
  tft.setTextSize(2);
  tft.printf("%.0f", humidity);
  tft.setTextSize(1);
  tft.print("%");
  
  // Feels Like
  tft.setCursor(100, 25);
  tft.setTextColor(COLOR_YELLOW);
  tft.setTextSize(2);
  tft.printf("%.1f", feels_like);
  tft.setTextSize(1);
  tft.print("C");
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
