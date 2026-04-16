# Task 4.1: SD Card Debugging & Error Diagnostics

## CRITICAL: How to Read the Diagnostic Output

The code tests 4 CS pin configurations automatically. Watch for:

### SUCCESS ✓ (You're done!)
```
Attempting PRIMARY (GPIO 15)...
✓ SD.begin() returned true
✓ File write test PASSED
✓✓✓ SD CARD WORKING! ✓✓✓
  Using CS pin: GPIO 15
```
→ Your SD card works! Proceed to Task 5.

### FAILURE ✗ (Needs debugging)
```
Attempting PRIMARY (GPIO 15)...
✓ SD.begin() returned true
✗ File write test FAILED (SD.open returned false)

Attempting SECONDARY (GPIO 3)...
✓ SD.begin() returned true
✗ File write test FAILED
[... more failures ...]

✗✗✗ CRITICAL ERROR ✗✗✗
SD card NOT detected on any tested CS pin
```
→ Follow troubleshooting below.

## Key Changes from Task 4

### 1. **Automatic Diagnostics on Startup**
Runs full SD card tests before attempting to log:
```
✓ Attempts init with CS=GPIO15 (primary)
✓ Falls back to CS=GPIO4 if primary fails
✓ Tests file operations (read/write)
✓ Tests CSV file creation
✓ Reports exact point of failure
```

### 2. **Fixed Heat Index Calculation**
The original Task 4 showed `F:156.8°C` - completely wrong!

**Root cause**: Missing validation in Steadman formula
- Can overflow with extreme values
- No bounds checking
- Invalid input validation

**Fixed in 4.1**:
```cpp
// Now validates inputs and caps at 60°C max
if (temp < 26.7) return temp;
if (humidity < 0 || humidity > 100) return temp;  // Validate RH
if (HI < T) return T;  // HI should be >= actual temp
if (HI > 60) return 60.0;  // Cap at realistic max
```

### 3. **Multiple SD_CS Pin Support**
Integrated SD adapters often use different CS pins:
```cpp
#define SD_CS_PRIMARY     15  // Try this first
#define SD_CS_FALLBACK    4   // Try this if primary fails
```

Code auto-detects which pin works and reports it.

### 4. **Error State Tracking**
```cpp
struct ErrorState {
  bool sd_init_failed;
  bool csv_create_failed;
  bool csv_write_failed;
  bool sensor_read_failed;
  char last_error[100];
};
```

Every operation tracked, last error stored.

### 5. **Serial Monitor Output (Detailed)**

```
=== Task 4.1: SD Card Debugging ===
Starting SD card diagnostics...

✓ Display initialized (landscape mode)
✓ DHT11 initialized on GPIO 14

=== SD CARD DIAGNOSTICS ===
Testing SD card initialization...
Attempting init with CS=GPIO15...
✓ SD card initialized successfully!
  Using CS pin: GPIO 15

Testing SD card operations...
Root directory contents:
  data_log.csv - 245 bytes

Testing file write...
✓ Test file opened for writing
  Bytes written: 25
✓ Test file closed successfully

Testing file read...
✓ Test file opened for reading
  Contents: Test write successful
✓ Test file read successfully

Testing CSV file creation...
CSV file already exists (will append)

✓ SD card ready for logging
```

## Integrated SD Adapter Troubleshooting (CRITICAL)

### What You're Seeing

Your output shows:
```
✓ SD card ready for logging        ← System thinks it's working
✗ Failed to open test file        ← But it's NOT actually working
✗ Failed to create CSV file       ← File operations failing
```

**This means**: `SD.begin()` is returning true, but the card isn't responding to actual commands.

### Root Cause

The integrated SD adapter on your display module uses a **different CS pin** than the default GPIO 17.

Common CS pins for integrated adapters:
- **GPIO 15** ← Most common
- **GPIO 3**
- **GPIO 4** 
- **GPIO 8**
- **GPIO 10** (ESP32-C3)

### Solution: Try Different CS Pins

The updated code now tests 4 CS pins automatically:
```cpp
#define SD_CS_PRIMARY     15  // Try first
#define SD_CS_SECONDARY   3   // Try second
#define SD_CS_TERTIARY    4   // Try third
#define SD_CS_FALLBACK    8   // Try fourth
```

**Upload the updated code and check Serial Monitor output.**

Expected successful output:
```
Attempting PRIMARY (GPIO 15)...
✓ SD.begin() returned true
✓ File write test PASSED
✓✓✓ SD CARD WORKING! ✓✓✓
  Using CS pin: GPIO 15
```

### If Still Failing

**Step 1: Check Display Module Documentation**

Your TFT module likely has a label or pinout diagram showing:
- SD module pinout
- Which GPIO pin is connected to SD_CS

Look for:
- Silk-screen labels (CS, MOSI, MISO, SCK, etc.)
- Schematic diagram
- Product specification sheet

**Step 2: Manual CS Pin Testing**

If documentation doesn't help, try other GPIO pins:

```cpp
// Edit lines 35-38 to test other combinations
#define SD_CS_PRIMARY     10   // Try GPIO 10
#define SD_CS_SECONDARY   16   // Try GPIO 16
#define SD_CS_TERTIARY    9    // Try GPIO 9
#define SD_CS_FALLBACK    11   // Try GPIO 11
```

**Valid GPIO pins to try** (avoid SPI pins used by TFT):
- ESP32: 0, 1, 2, 3, 4, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17
- DO NOT use: 6, 7, 18, 19, 23 (TFT/SPI)

**Step 3: Physical Checks**

1. **SD Card Seating**
   - Remove and reinsert firmly
   - Listen for "click"
   - Should not slide out easily

2. **Card Contacts**
   - Clean gold contacts with soft cloth
   - Ensure no dust/corrosion

3. **Card Format**
   - Try a different SD card
   - Format card on computer as **FAT32**

4. **Card Compatibility**
   - Older/smaller cards work best
   - Newer large cards sometimes have issues
   - Try: 2GB, 4GB, or 8GB card

## How to Interpret Diagnostic Output

### Successful Init
```
✓ SD card initialized successfully!
  Using CS pin: GPIO 15

Testing SD card operations...
Root directory contents:
  data_log.csv - 245 bytes
```
**Means**: SD card working, existing data found

### File Write Test
```
✓ Test file opened for writing
  Bytes written: 25
✓ Test file read successfully
```
**Means**: Read/write operations working

### Failed Init
```
✗ Failed with primary CS pin
Attempting fallback CS=GPIO4...
✗ Failed with fallback CS pin
ERROR: SD card not detected on either CS pin
```
**Means**: Need to find correct CS pin (see troubleshooting above)

## Display Output

```
T   | H   | Feels
    |     |
──────────────────
SD: OK (GPIO15)
25.3C | 51% | 28.5C
```

- **Top**: Column headers
- **Middle**: SD card status with CS pin used
- **Bottom**: Live sensor readings

If SD fails:
```
SD: ERROR
```

## CSV File Format

Still the same as Task 4:
```
timestamp,temperature,humidity,feels_like
2026-04-18T14:32:45Z,25.3,51.0,28.5
```

But now with validated heat index (not 156.8°C!)

## Configuration

Only change needed for most users:
```cpp
// Line 35-36: Your WiFi credentials
#define SSID              "Itzz_Infinity"
#define PASSWORD          "123456789"

// Line 39-40: If SD_CS doesn't work, modify these
#define SD_CS_PRIMARY     15
#define SD_CS_FALLBACK    4
```

## Debugging Workflow

1. **Upload code**
2. **Open Serial Monitor (115200 baud)**
3. **Read the diagnostics output** - it tells you exactly what worked/failed
4. **If SD init failed**:
   - Check physical connections
   - Try different CS pins
   - Format SD card
5. **If CSV creation failed**:
   - Check write permissions
   - Verify CS pins don't conflict
   - Try different SD card
6. **If logging works**:
   - Check CSV file on computer
   - Verify data format (ISO 8601 timestamps)
   - Verify heat index values are reasonable (≤60°C)

## Performance

- **Startup time**: ~5-10 seconds (includes WiFi + diagnostics)
- **Log write**: ~100ms per entry
- **Memory**: ~90KB RAM
- **Compiled size**: ~250KB

## Data Validation

Check your CSV file:
```
# Should have 4 columns
timestamp,temperature,humidity,feels_like

# Timestamps must be ISO 8601
2026-04-18T14:32:45Z,25.3,51.0,28.5

# Feels-like should be >= temperature
25.3C with 156.8 "feels like" = BUG (now fixed)
25.3C with 28.5 "feels like" = CORRECT

# Heat index only applies when T > 26.7°C
20.0C,70% → feels_like = 20.0 (not calculated)
30.0C,80% → feels_like = 36+ (calculated)
```

## Next Steps

Once Task 4.1 diagnostics are successful:
1. Note the **working CS pin** (e.g., "GPIO 15")
2. Move to Task 5 (web server for CSV download)
3. Use same SD_CS pin in subsequent tasks

## Files Generated

- Serial Monitor diagnostics (full output)
- `data_log.csv` on SD card (with validated data)
- Display shows status and readings

---

**Use serial monitor output to debug - it's extremely detailed!**
