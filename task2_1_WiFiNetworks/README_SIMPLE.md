# WiFi Network Scanner - Simple Version

## What It Does

A minimal WiFi network scanner that:
- ✅ Scans available WiFi networks every 10 seconds
- ✅ Displays networks with signal strength bars
- ✅ Shows RSSI (dBm) for each network
- ✅ Pagination for >5 networks
- ❌ No WiFi connection attempt
- ❌ No time/date display
- ❌ No login credentials needed

## Display Layout

```
WiFi Networks: 3
────────────────────────────
Network1         #### -45dBm
Network2         ###  -62dBm
Network3         #    -78dBm
Network4         -    -92dBm
Network5         -   -100dBm

           Page 1/1
Scanning...
```

## File Comparison

| Feature | Original | Simple |
|---------|----------|--------|
| Lines of Code | ~500 | ~200 |
| WiFi Connection | Yes | No |
| Time Display | Yes | No |
| Network Scanning | Yes | Yes |
| Signal Bars | Yes | Yes |
| Date/Time | Yes | No |
| Complexity | High | Minimal |

## How to Use

### 1. Upload Code
```
Arduino IDE:
  1. Select Board: ESP32 or ESP32-C3
  2. Select Port: Your USB port
  3. Copy code from task2_1_WiFiNetworks_simple.ino
  4. Upload
```

### 2. Monitor Results
Open Serial Monitor (Tools → Serial Monitor, 115200 baud)

You'll see:
```
WiFi Network Scanner - Starting...
✓ Display & WiFi initialized

Scanning for WiFi networks...
  [1] Itzz_Infinity (-45 dBm)
  [2] OtherNetwork (-72 dBm)
  [3] Guest (-88 dBm)
```

### 3. View Display
TFT LCD shows:
- List of available networks
- Signal strength for each (# = strong, - = weak)
- RSSI in dBm (closer to 0 is stronger)
- Page indicator if >5 networks

## Configuration

### Change Scan Interval
```cpp
Line 47:
#define NETWORK_SCAN_INTERVAL 10000  // 10000 ms = 10 seconds
// Change to 5000 for 5 second intervals
```

### Change Networks Per Page
```cpp
Line 48:
#define NETWORKS_PER_PAGE 5  // Show 5 networks at a time
// Change to 3 for compact view, 8 for more networks
```

## Signal Strength Reference

```
[####]  ≥ -50 dBm   Excellent
[###]   -50 to -60  Good
[##]    -60 to -70  Fair
[#]     -70 to -80  Weak
[-]     ≤ -80 dBm   Very Weak
```

## Serial Output

Expected output on startup:
```
WiFi Network Scanner - Starting...
✓ Display & WiFi initialized

Scanning for WiFi networks...
  [1] Itzz_Infinity (-45 dBm)
  [2] OtherNetwork (-62 dBm)
  [3] Guest (-78 dBm)

Scanning for WiFi networks...
  [1] Itzz_Infinity (-44 dBm)
  [2] OtherNetwork (-63 dBm)
  [3] Guest (-79 dBm)
```

Updates every 10 seconds automatically.

## Code Size

- **Compiled Size**: ~150 KB
- **RAM Used**: ~50 KB
- **Upload Time**: ~5 seconds

## Hardware Compatibility

### ESP32 (38-pin)
```
TFT_CS   = GPIO 5
TFT_RST  = GPIO 4
TFT_DC   = GPIO 2
TFT_MOSI = GPIO 23
TFT_SCK  = GPIO 18
```

### ESP32-C3 (25-pin)
```
TFT_CS   = GPIO 7
TFT_RST  = GPIO 8
TFT_DC   = GPIO 10
TFT_MOSI = GPIO 6
TFT_SCK  = GPIO 4
```

Auto-detected in code - no changes needed.

## Troubleshooting

| Issue | Solution |
|-------|----------|
| Blank display | Check TFT wiring (same as Task 1) |
| No networks shown | Check WiFi antenna, try changing location |
| Display upside down | Change `tft.setRotation(0)` to `tft.setRotation(1)` or `(2)` |
| Crashes on upload | Check board selection (ESP32 vs ESP32-C3) |
| Garbled text | Verify pin definitions for your board |

## What Data Is Collected

For each network:
- SSID (network name)
- RSSI (signal strength in dBm)
- Channel number (WiFi channel)

**No data is transmitted or stored.**

## Minimal Dependencies

Required:
- ✓ Adafruit_ST7735 (display)
- ✓ Adafruit_GFX (graphics)
- ✓ WiFi.h (built-in)
- ✓ SPI.h (built-in)

Optional:
- ❌ WiFi connection libraries
- ❌ NTP/time libraries
- ❌ SD card libraries

## Example: How to Extend

Add relay control based on network strength:

```cpp
// After displayNetworks(), add:
if (networks[0].rssi >= -50) {
  Serial.println("Strong signal - relay ON");
} else {
  Serial.println("Weak signal - relay OFF");
}
```

## Performance

- **Scan Time**: ~1-2 seconds
- **Display Update**: <100ms (smooth, no flicker)
- **Energy**: Very low - just scanning, no transmission
- **Responsiveness**: Real-time updates every 10 seconds

## This vs Original Task 2.1

**Use SIMPLE version if you want:**
- ✓ Just network scanning
- ✓ Minimal code
- ✓ No WiFi connection issues
- ✓ Fast learning/testing

**Use ORIGINAL (task2_1_WiFiNetworks.ino) if you want:**
- ✓ WiFi connection + time sync
- ✓ Full featured
- ✓ Real-time clock
- ✓ Connection status

---

**Ready to upload?** Copy `task2_1_WiFiNetworks_simple.ino` contents to Arduino IDE and upload!
