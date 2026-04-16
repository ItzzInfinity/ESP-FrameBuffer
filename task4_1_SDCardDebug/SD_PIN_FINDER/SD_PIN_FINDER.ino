/*
 * ============================================================================
 * UTILITY: SD Card CS Pin Finder
 * ============================================================================
 * PURPOSE:
 *   Systematically test all GPIO pins to find which one is the correct
 *   CS pin for your integrated SD card adapter.
 *
 * HOW TO USE:
 *   1. Upload this sketch
 *   2. Open Serial Monitor (115200 baud)
 *   3. Watch the output - it tests pins one by one
 *   4. When you see "✓✓✓ WORKING! ✓✓✓", note the GPIO number
 *   5. Use that GPIO as SD_CS_PRIMARY in main code
 *
 * PINS TESTED:
 *   ESP32: 0, 1, 2, 3, 4, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17
 *   (Skips: 5, 6, 7, 18, 19, 23 which are used by TFT/SPI)
 *
 * ============================================================================
 */

#include <SD.h>
#include <SPI.h>

// Pins to test (ESP32)
int test_pins[] = {0, 1, 2, 3, 4, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17};
const int num_pins = sizeof(test_pins) / sizeof(test_pins[0]);

void setup() {
  Serial.begin(115200);
  delay(2000);
  
  Serial.println("\n\n=== SD Card CS Pin Finder ===");
  Serial.println("Testing GPIO pins to find correct CS pin...\n");
  Serial.println("Testing: ");
  for (int i = 0; i < num_pins; i++) {
    Serial.print(test_pins[i]);
    if (i < num_pins - 1) Serial.print(", ");
  }
  Serial.println("\n");
  
  // Test each pin
  for (int i = 0; i < num_pins; i++) {
    int cs_pin = test_pins[i];
    testPin(cs_pin);
  }
  
  Serial.println("\n=== Testing Complete ===");
  Serial.println("If no GPIO showed ✓✓✓ WORKING, check:");
  Serial.println("1. SD card is fully inserted");
  Serial.println("2. SD card contacts are clean");
  Serial.println("3. Try a different SD card");
  Serial.println("4. Try different GPIO pins manually");
}

void loop() {
  delay(1000);
}

void testPin(int cs_pin) {
  Serial.print("Testing GPIO ");
  Serial.print(cs_pin);
  Serial.print("... ");
  
  // Try to initialize with this CS pin
  if (SD.begin(cs_pin)) {
    Serial.println("SD.begin() returned true");
    
    // CRITICAL TEST: Try to open a file
    File test = SD.open("test.txt", FILE_WRITE);
    if (test) {
      Serial.print("  ✓ File write succeeded");
      test.println("CS pin " + String(cs_pin) + " works!");
      test.close();
      
      Serial.println(" - File write successful!");
      Serial.println("  ╔════════════════════════════════════════╗");
      Serial.println("  ║ ✓✓✓ SD CARD WORKING! ✓✓✓              ║");
      Serial.println("  ║ CS PIN: GPIO " + String(cs_pin) + (cs_pin < 10 ? "                     " : "                    ") + "║");
      Serial.println("  ║                                        ║");
      Serial.println("  ║ Use this in your code:                 ║");
      Serial.println("  ║ #define SD_CS_PRIMARY " + String(cs_pin) + (cs_pin < 10 ? "                ║" : "               ║"));
      Serial.println("  ╚════════════════════════════════════════╝");
      
      // List files
      File root = SD.open("/");
      Serial.println("\n  Files on card:");
      listDir(root, "    ");
      root.close();
      
      Serial.println();
      return;
    } else {
      Serial.println("SD.begin() ok, but file write FAILED");
    }
  } else {
    Serial.println("SD.begin() failed");
  }
}

void listDir(File dir, String prefix) {
  while (true) {
    File entry = dir.openNextFile();
    if (!entry) break;
    
    Serial.print(prefix);
    Serial.print(entry.name());
    
    if (entry.isDirectory()) {
      Serial.println("/");
    } else {
      Serial.print(" (");
      Serial.print(entry.size());
      Serial.println(" bytes)");
    }
    
    entry.close();
  }
}
