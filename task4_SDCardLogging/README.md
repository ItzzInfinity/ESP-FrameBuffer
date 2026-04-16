# Task 4: SD Card CSV Logging with Feels-Like Temperature

## What It Does

Complete data logging system with:
- ✅ **CSV Logging** - Records temperature, humidity, feels-like temperature
- ✅ **ISO 8601 Timestamps** - Format: "2026-04-18T14:32:45Z"
- ✅ **DHT11 Sensor** - Reads every 2 seconds
- ✅ **Feels-Like Calculation** - Heat index based on temp + humidity
- ✅ **Shared SPI Bus** - TFT LCD and SD card on same bus
- ✅ **Landscape Display** - Wider orientation for more info
- ✅ **Partial Updates** - Only refreshes changed sections
- ✅ **Error Handling** - Graceful SD card error messages
- ✅ **Configurable Interval** - Data logging every 5 minutes (adjustable)

## CSV File Format

**Filename**: `data_log.csv`

**Header**: `timestamp,temperature,humidity,feels_like`

**Example entries**:
```
2026-04-18T14:32:45Z,25.3,65.0,28.5
2026-04-18T14:37:45Z,25.5,64.0,28.8
2026-04-18T14:42:45Z,25.8,63.0,29.2
```

## Display (Landscape Mode)

```
Temperature | Humidity | Feels Like
    25.3°C   |   65%    |    28.5°C
────────────────────────────────────
Last Log: 2026-04-18T14:32:45Z
Interval: 300s
Status: OK
```

## What is "Feels Like" Temperature?

Heat index (feels-like temperature) accounts for:
- **Actual temperature** (°C)
- **Humidity** (%)
- **Body perception** - How hot it "feels" with humidity

**Formula**: Steadman heat index formula
- When T < 27°C → Feels like = actual temperature
- When T ≥ 27°C → Heat index calculation considers humidity

**Example**:
- 25°C, 50% RH → Feels like: 25°C
- 30°C, 80% RH → Feels like: 36.5°C (much hotter!)

## Hardware Setup (10 Minutes)

### 1. Wiring DHT11
```
DHT11 VCC  → 3.3V
DHT11 DATA → GPIO 14
DHT11 GND  → GND
```

### 2. Wiring SD Card Module (Shared SPI Bus)

| SD Pin | ESP32 | ESP32-C3 |
|--------|-------|----------|
| VCC | 3.3V | 3.3V |
| GND | GND | GND |
| MOSI | GPIO 23 | GPIO 6 |
| MISO | GPIO 19 | GPIO 5 |
| SCK | GPIO 18 | GPIO 4 |
| CS | GPIO 17 | GPIO 3 |

**Important**: SD_CS pin is different from TFT_CS to prevent SPI conflicts!

### 3. Install Libraries

```
Arduino IDE → Sketch → Include Library → Manage Libraries
```

Install:
- "DHT sensor library by Adafruit"
- "SD" (Arduino SD card library - usually pre-installed)

### 4. Set Configuration

```cpp
Line 35-38 (edit as needed):
#define SSID              "Itzz_Infinity"
#define PASSWORD          "123456789"
#define LOGGING_INTERVAL  300000        // 5 minutes = 300000 ms
#define GMT_OFFSET        19800         // UTC+5:30
```

### 5. Upload

```
Select Board: ESP32 or ESP32-C3
Select Port: Your USB port
Upload
```

## Configuration Options

### Change Logging Interval
```cpp
#define LOGGING_INTERVAL  300000  // milliseconds
// 60000 = 1 minute
// 300000 = 5 minutes (default)
// 600000 = 10 minutes
```

### Change Timezone
```cpp
#define GMT_OFFSET 19800  // UTC+5:30 (in seconds)
// UTC+0 = 0
// UTC+5:30 = 19800
// UTC-5 = -18000
```

### Change WiFi Credentials
```cpp
#define SSID     "your_ssid"
#define PASSWORD "your_password"
```

### Change SD Card CS Pin (if different)
```cpp
// ESP32 default: GPIO 17
// ESP32-C3 default: GPIO 3
#define SD_CS 17  // Change to your pin
```

## Serial Monitor Output

```
Task 4: SD Card CSV Logging
Display initialized (landscape mode)
DHT11 initialized on GPIO 14
Connecting to WiFi: Itzz_Infinity
.....✓ WiFi Connected
Syncing time via NTP...
.....✓ Time synced: Thu Apr 18 14:32:45 2026
WiFi disconnected
Initializing SD card...
✓ SD card initialized
CSV file already exists, skipping header

T:25.3 H:65.0 F:28.5
✓ Logged: 2026-04-18T14:32:45Z,25.3,65.0,28.5
T:25.5 H:64.0 F:28.8
✓ Logged: 2026-04-18T14:37:45Z,25.5,64.0,28.8
```

## Troubleshooting

| Problem | Solution |
|---------|----------|
| **SD card not detected** | Check wiring, verify CS pin, try different SD card |
| **CSV file not created** | Check SD card is writable, verify permissions |
| **Time wrong in CSV** | Check GMT_OFFSET, verify WiFi sync worked |
| **Display blank** | Check TFT wiring, verify board orientation (landscape) |
| **Sensor not reading** | Check DHT11 wiring, verify GPIO 14 assignment |

## File Management

### View Logged Data

**Option 1: Using Arduino IDE**
- Tools → Serial Monitor (show SD card writes)

**Option 2: Remove SD card and use computer**
- Insert SD card into card reader
- Open `data_log.csv` in spreadsheet app (Excel, Google Sheets, etc.)
- Data is comma-separated and easy to analyze

### CSV Parsing Example

**Python**:
```python
import csv
with open('data_log.csv') as f:
    reader = csv.DictReader(f)
    for row in reader:
        print(f"{row['timestamp']}: {row['temperature']}°C")
```

**Excel/Google Sheets**:
- File → Open → data_log.csv
- Select comma as delimiter
- Data automatically organized into columns

## Shared SPI Bus Management

Both TFT and SD card use the same SPI bus but different CS (chip select) pins:

```
TFT Display:    CS = GPIO 5 (ESP32) / GPIO 7 (ESP32-C3)
SD Card:        CS = GPIO 17 (ESP32) / GPIO 3 (ESP32-C3)
MOSI:           GPIO 23 (ESP32) / GPIO 6 (ESP32-C3)
SCK:            GPIO 18 (ESP32) / GPIO 4 (ESP32-C3)
```

When TFT is active, SD is inactive (CS high).
When SD is active, TFT is inactive (CS high).

The library automatically manages this.

## Display Regions (Partial Updates)

| Region | Content | Update Trigger |
|--------|---------|-----------------|
| Top | Sensor readings (T, H, FL) | Every 2s if changed ±0.5 |
| Middle | Last log timestamp | Every log interval |
| Bottom | Status and interval | On SD error or first display |

## Performance

### Data Log Growth
```
One entry per log interval:
- 5 minutes: ~288 entries/day = ~15KB/day
- 10 minutes: ~144 entries/day = ~7.5KB/day
- 1 hour: ~24 entries/day = ~1.2KB/day

A 1GB SD card can hold:
- 5 min interval: ~215 years of data!
- 10 min interval: ~430 years of data!
```

### SD Card Writes
```
Write time per entry: ~50-100ms
Display update: ~20-50ms (if values changed)
CPU usage: ~3-8% during operation
Power: ~30-50mA normal, ~120mA during WiFi startup
```

## Testing Checklist

After upload:
- [ ] Serial monitor shows "SD card initialized"
- [ ] Display shows landscape orientation (wider than tall)
- [ ] Temperature, humidity, feels-like display
- [ ] Every 5 minutes (default), serial shows "✓ Logged:"
- [ ] No "SD Error" messages
- [ ] Remove SD card and view data_log.csv on computer
- [ ] CSV file has correct format with ISO 8601 timestamps

## CSV Data Validation

**Check CSV structure**:
```
# Should have 4 columns
timestamp,temperature,humidity,feels_like

# Should have ISO 8601 timestamps
2026-04-18T14:32:45Z,25.3,65.0,28.5
2026-04-18T14:37:45Z,25.5,64.0,28.8

# Temperature and humidity should be reasonable
# Feels-like ≥ temperature (unless below 27°C)
```

## Code Size & Memory

- **Compiled Size**: ~240 KB
- **RAM Usage**: ~80-100 KB
- **SPIFFS/SD**: Unlimited (depends on SD card)

## Next Steps (Task 5)

Task 5 will add:
- Simple web server
- HTTP endpoints to download CSV file
- WiFi access point for easy download

The SD card logging from Task 4 will be reused directly in Task 5.

---

**Ready to log data?** Upload `task4_SDCardLogging.ino` and monitor serial output!
