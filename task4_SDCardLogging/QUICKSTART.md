# Task 4: Quick Start (10 Minutes)

## Wire It Up (5 Minutes)

### DHT11
```
DHT11 VCC  → 3.3V
DHT11 DATA → GPIO 14
DHT11 GND  → GND
```

### SD Card (Shared SPI Bus)
```
SD VCC     → 3.3V
SD GND     → GND
SD MOSI    → GPIO 23 (ESP32) / GPIO 6 (ESP32-C3)
SD MISO    → GPIO 19 (ESP32) / GPIO 5 (ESP32-C3)
SD SCK     → GPIO 18 (ESP32) / GPIO 4 (ESP32-C3)
SD CS      → GPIO 17 (ESP32) / GPIO 3 (ESP32-C3)
```

## Install Library (2 Minutes)

Arduino IDE:
```
Sketch → Include Library → Manage Libraries
Search: "DHT sensor library by Adafruit"
Click: Install

Note: SD library usually pre-installed
```

## Upload Code (3 Minutes)

1. Copy code from `task4_SDCardLogging.ino`
2. Arduino IDE: Select Board (ESP32 / ESP32-C3)
3. Select Port (your USB)
4. Upload

## Verify (30 Seconds)

Open Serial Monitor (115200 baud):
```
Expected output:
✓ WiFi Connected
✓ Time synced
✓ SD card initialized
✓ Logged: 2026-04-18T14:32:45Z,25.3,65.0,28.5
```

Check TFT Display:
```
Temperature | Humidity | Feels Like
    25.3°C   |   65%    |    28.5°C
```

## Check CSV File

After 5 minutes (default logging interval):

1. **Remove SD card**
2. **Insert into computer's card reader**
3. **Open `data_log.csv`** with:
   - Excel
   - Google Sheets
   - Text editor

**File should contain**:
```
timestamp,temperature,humidity,feels_like
2026-04-18T14:32:45Z,25.3,65.0,28.5
2026-04-18T14:37:45Z,25.5,64.0,28.8
```

## Done! ✓

System will:
- Log data every 5 minutes (configurable)
- Display current readings on TFT
- Write to SD card with ISO 8601 timestamps
- Handle errors gracefully

---

## Quick Config Changes

### Change Logging Interval
```cpp
Line 37:
#define LOGGING_INTERVAL  300000  // milliseconds
// 60000 = 1 minute
// 300000 = 5 minutes (default)
```

### Change Timezone
```cpp
Line 38:
#define GMT_OFFSET 19800  // UTC+5:30
```

### Change WiFi
```cpp
Line 35-36:
#define SSID     "your_ssid"
#define PASSWORD "your_password"
```

---

**See README.md for full setup, troubleshooting, and data analysis**
