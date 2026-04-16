# PIN FINDER Utility - Find Your SD Card CS Pin

## When to Use This

Use `SD_PIN_FINDER.ino` when:
- The main diagnostic code tests 4 pins but none work
- You need to systematically test all available GPIO pins
- You have no documentation for your display module

## How It Works

1. Systematically tests GPIO pins: 0, 1, 2, 3, 4, 8-17
2. For each pin, attempts to:
   - Initialize SD card: `SD.begin(pin)`
   - Write a test file
   - Read the file back
3. **Stops at first working pin** and displays the result clearly

## Usage

### Step 1: Upload PIN FINDER
```
1. Open SD_PIN_FINDER.ino in Arduino IDE
2. Select Board: ESP32
3. Select Port: Your COM port
4. Click Upload
```

### Step 2: Monitor the Output
Open Serial Monitor (Tools → Serial Monitor, 115200 baud)

You'll see:
```
=== SD Card CS Pin Finder ===
Testing GPIO pins to find correct CS pin...

Testing: 0, 1, 2, 3, 4, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17

Testing GPIO 0... SD.begin() failed
Testing GPIO 1... SD.begin() failed
Testing GPIO 2... SD.begin() failed
Testing GPIO 3... SD.begin() ok, but file write FAILED
Testing GPIO 4... SD.begin() ok, but file write FAILED
Testing GPIO 8... SD.begin() ok, but file write FAILED
Testing GPIO 9... SD.begin() ok, but file write FAILED
Testing GPIO 10... SD.begin() ok, but file write FAILED
Testing GPIO 11... SD.begin() ok, but file write FAILED
Testing GPIO 12... SD.begin() ok, but file write FAILED
Testing GPIO 13... SD.begin() ok, but file write FAILED
Testing GPIO 14... SD.begin() failed
Testing GPIO 15... SD.begin() returned true
  ✓ File write succeeded
  ╔════════════════════════════════════════╗
  ║ ✓✓✓ SD CARD WORKING! ✓✓✓              ║
  ║ CS PIN: GPIO 15                        ║
  ║                                        ║
  ║ Use this in your code:                 ║
  ║ #define SD_CS_PRIMARY 15               ║
  ╚════════════════════════════════════════╝
  
  Files on card:
    data_log.csv (245 bytes)
    test.txt (31 bytes)
```

### Step 3: Note the Working GPIO

When you see `✓✓✓ SD CARD WORKING!`, write down the GPIO number.

Example: **GPIO 15**

### Step 4: Update Main Code

Edit `task4_1_SDCardDebug.ino` lines 35-36:
```cpp
#define SD_CS_PRIMARY     15   // ← Use the GPIO from PIN FINDER
#define SD_CS_SECONDARY   3
#define SD_CS_TERTIARY    4
#define SD_CS_FALLBACK    8
```

Or simplify (once you know it works):
```cpp
#define SD_CS_PRIMARY     15   // ← Only need this one
#define SD_CS_SECONDARY   0    // Dummy value (won't be used)
#define SD_CS_TERTIARY    0
#define SD_CS_FALLBACK    0
```

### Step 5: Re-upload Main Code

Upload `task4_1_SDCardDebug.ino` with the correct CS pin.

## Expected Output Patterns

### Pattern 1: All Failed
```
Testing GPIO X... SD.begin() failed
Testing GPIO Y... SD.begin() failed
[... more failures ...]
```
**Means**: Card not connected or incompatible. Check:
- SD card fully inserted
- Card contacts clean
- Correct display module

### Pattern 2: Some Pass, but Write Fails
```
Testing GPIO X... SD.begin() ok, but file write FAILED
Testing GPIO Y... SD.begin() ok, but file write FAILED
```
**Means**: Card is detected but not responding to commands. Try:
- Different SD card
- Format card as FAT32
- Re-seat card firmly

### Pattern 3: Success
```
Testing GPIO X... SD.begin() returned true
  ✓ File write succeeded
  ✓✓✓ SD CARD WORKING! ✓✓✓
  CS PIN: GPIO X
```
**Means**: Found it! Use GPIO X as your CS_PRIMARY

## Tested Pins

| GPIO | Status | Notes |
|------|--------|-------|
| 5 | Skipped | Used by TFT (VSPI_CS) |
| 6, 7 | Skipped | Used by TFT/SDIO (internal) |
| 18 | Skipped | Used by TFT (VSPI_CLK) |
| 19, 23 | Skipped | Used by TFT/SPI |
| 0-4, 8-17 | Tested | Available for CS pin |

## Troubleshooting PIN FINDER

| Issue | Solution |
|-------|----------|
| Infinite loop | Sketch is still running, all pins tested |
| No success message | SD card issue, not GPIO configuration |
| All pins fail | Try different SD card or format card |
| Works in PIN FINDER but not main code | Ensure both use same CS pin |

## Next Steps

Once PIN FINDER finds your CS pin:
1. Update main code with correct GPIO
2. Upload `task4_1_SDCardDebug.ino`
3. Should immediately show "✓ SD card ready for logging"
4. Proceed to Task 5 (web server)

---

**PIN FINDER helps solve integrated SD adapter mysteries!**
