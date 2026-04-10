# Task 2: Internet Time Display with WiFi Network Array

## Objective
Connect ESP32/ESP32C3 to internet via WiFi, synchronize time using NTP (Network Time Protocol), and display live time/date on the TFT LCD display.

## Key Features

### ✅ WiFi Network Array (Flexible Multi-Network Support)
Instead of a single hardcoded SSID/password, this code uses an **array of networks** that allows:
- Multiple WiFi networks (home, office, mobile hotspot, etc.)
- Automatic fallback if primary network unavailable
- Easy to add/remove networks without modifying core logic
- Auto-scanning for available networks before attempting connection

### ✅ Automatic Time Synchronization
- Connects to NTP server (pool.ntp.org) after WiFi connection
- Synchronizes system clock automatically
- Supports timezone configuration
- Supports daylight saving time offset

### ✅ Real-Time Display Updates
- Updates time display every 1 second
- Shows current date with day of week
- Displays WiFi status (connected/disconnected)
- Shows network name and IP address
- Displays WiFi signal strength (RSSI)

### ✅ Error Handling
- Graceful fallback if all networks fail
- Automatic reconnection attempts
- Clear error messages on display
- Serial monitoring for debugging

---

## Hardware Setup

Same as Task 1:

| TFT Pin | Function      | ESP32 GPIO | ESP32-C3 GPIO |
|---------|---------------|-----------|---------------|
| VCC     | Power (3.3V)  | 3V3       | 3V3           |
| GND     | Ground        | GND       | GND           |
| CS      | Chip Select   | GPIO 5    | GPIO 7        |
| RESET   | Reset         | GPIO 4    | GPIO 8        |
| A0/DC   | Data/Command  | GPIO 2    | GPIO 10       |
| SDA     | MOSI (Data)   | GPIO 23   | GPIO 6        |
| SCK     | Clock         | GPIO 18   | GPIO 4        |
| LED     | Backlight     | 3V3       | 3V3           |

---

## WiFi Network Configuration

### Adding Your Networks

**Option 1: Single Network (Your Credentials)**

The code comes pre-configured with your network:
```cpp
const WiFiNetwork WIFI_NETWORKS[] = {
  {"Itzz_Infinity", "123456789"},  // Pre-configured
};
```

**Option 2: Multiple Networks (Recommended)**

Add additional networks by uncommenting and filling:
```cpp
const WiFiNetwork WIFI_NETWORKS[] = {
  {"Itzz_Infinity", "123456789"},        // Primary network (home)
  {"MobileHotspot", "hotspot_password"}, // Secondary (mobile)
  {"Office_WiFi", "office_password"},    // Tertiary (office)
  {"GuestNetwork", "guest_password"},    // Backup
};
```

**How It Works:**
1. System scans for available WiFi networks
2. Tries each network in WIFI_NETWORKS array order
3. Connects to first available network found
4. If connection drops, automatically reconnects (with fallback to other networks)

**Important:**
- Networks are tried in the order they appear in the array
- The array is checked against scan results (only available networks are attempted)
- No more "connection failed" - the system adapts to available networks
- Passwords must be exactly 8-63 characters (WiFi standard)

---

## Time Configuration

### Timezone Setting

Modify `GMT_OFFSET_SEC` in the code to match your timezone:

```cpp
#define GMT_OFFSET_SEC 0              // UTC+0
```

**Common Timezone Values (in seconds):**
- UTC+0 (London, GMT):       `0`
- UTC+1 (Berlin, Paris):     `3600`
- UTC+2 (Cairo, Helsinki):   `7200`
- UTC+5:30 (India, IST):     `19800`
- UTC+8 (China, Singapore):  `28800`
- UTC-5 (New York, EST):     `-18000`
- UTC-8 (Los Angeles, PST):  `-28800`

### Daylight Saving Time

If your region observes DST, set:
```cpp
#define DST_OFFSET_SEC 3600  // 1 hour additional offset during DST
```

Otherwise, leave as `0`.

### NTP Server

Default server is `pool.ntp.org` (global NTP pool). Can be changed:
```cpp
#define NTP_SERVER "time.nist.gov"  // Alternative server
```

---

## Library Installation

1. **Adafruit ST7735** (for TFT display)
   - Arduino IDE → Sketch → Include Library → Manage Libraries
   - Search "Adafruit ST7735" → Install

2. **Adafruit GFX** (graphics library, auto-installs with ST7735)
   - Should install automatically with Adafruit ST7735

3. **WiFi.h & time.h**
   - Built-in with ESP32 board support (no additional install needed)

4. **ESP32 Board Support** (if not already installed)
   - Arduino IDE → File → Preferences
   - Paste in Additional Boards Manager URLs: `https://dl.espressif.com/dl/package_esp32_index.json`
   - Tools → Board Manager → Search "ESP32" → Install "esp32 by Espressif Systems"

---

## Setup & Upload

### Arduino IDE Configuration

**For ESP32:**
- Board: ESP32 Dev Module
- Upload Speed: 921600
- Partition Scheme: Huge APP (3MB No OTA)

**For ESP32-C3:**
- Board: ESP32-C3 Dev Module
- Upload Speed: 921600

### Upload Steps

1. Connect ESP32 to computer via USB
2. Open `task2_InternetTime.ino` in Arduino IDE
3. Tools → Select correct Board and Port
4. Click **Upload**
5. Open Tools → Serial Monitor (115200 baud)

### Expected Serial Output

```
===========================================
espFrameBuffer - Task 2: Internet Time
===========================================
Board: ESP32
WiFi Networks Available: 1
  [1] Itzz_Infinity
Initializing TFT display...
✓ Display initialized
Starting WiFi connection...

--- Starting WiFi Connection Sequence ---
Scanning available networks...
Networks found: 5
Attempting connection to: Itzz_Infinity
..........✓ Connected! IP: 192.168.1.100
Synchronizing time via NTP...
Waiting for NTP time sync... ✓ Time synchronized: Mon Jan 17 14:32:45 2026
```

---

## Display Output

### During Startup
Shows "Initializing..." with loading animation

### During Connection
Displays network name being connected to with "Please wait..." message

### Connected & Running
Shows:
- **Time** (large yellow text): `HH:MM:SS`
- **Date**: `Day Month Date Year`
- **WiFi Status**: Green "WiFi" if connected, Red if disconnected
- **Network Info**: Network name, IP address, Signal strength (RSSI in dBm)

### WiFi Disconnection
- Display updates automatically
- Shows "Disconnected - Attempting to connect..."
- System tries to reconnect to available networks

---

## Troubleshooting

| Issue | Solution |
|-------|----------|
| Display shows nothing | Check wiring (same as Task 1), verify CS/DC pins |
| WiFi not connecting | 1. Check WiFi password (must be 8-63 chars)<br>2. Verify SSID spelling matches exactly<br>3. Check router allows ESP32 devices<br>4. Try 2.4GHz WiFi (ESP32 doesn't support 5GHz)<br>5. Add backup networks to array |
| Time not syncing (stuck on default) | 1. Verify WiFi is connected<br>2. Try different NTP server<br>3. Check internet connection<br>4. Look for DNS resolution issues in Serial |
| Display artifacts at screen edges | Same fix as Task 1 - already applied with margin padding |
| WiFi connects but drops | 1. Check WiFi signal strength (RSSI)<br>2. Reduce signal interference<br>3. Try accessing from closer to router |
| Wrong time displayed | 1. Check timezone offset (GMT_OFFSET_SEC)<br>2. Verify NTP sync completed (check Serial)<br>3. Check if DST offset is correct |

---

## Code Structure

```
task2_InternetTime.ino
├── COLOR DEFINITIONS
├── WIFI CONFIGURATION (NETWORK ARRAY)
├── NTP TIME CONFIGURATION
├── PIN DEFINITIONS
├── GLOBAL VARIABLES
├── setup() - Initialize hardware & start WiFi
├── loop() - Periodic updates
├── WIFI FUNCTIONS
│   ├── connectToWiFi() - Auto-connect with fallback
│   └── syncTimeNTP() - Synchronize time
└── DISPLAY FUNCTIONS
    ├── displayInitScreen()
    ├── displayConnecting()
    ├── displayConnected()
    ├── displayError()
    └── displayTimeAndDate() - Main time display
```

---

## Advanced Configuration

### WiFi Connection Timeout
```cpp
int timeout = 20;  // 20 attempts × 500ms = 10 seconds per network
```
Adjust `timeout` value in `connectToWiFi()` to change per-network timeout.

### Display Update Frequency
```cpp
#define DISPLAY_UPDATE_INTERVAL 1000   // Update every 1000ms
#define WIFI_CHECK_INTERVAL 10000      // Check WiFi every 10000ms
```

### WiFi Scan Options
Add to `connectToWiFi()` before `WiFi.scanNetworks()`:
```cpp
WiFi.mode(WIFI_STA);
WiFi.setAutoConnect(true);     // Auto-connect to last known network
WiFi.setAutoReconnect(true);   // Auto-reconnect if disconnected
```

---

## Testing Checklist

- [ ] Code compiles without errors
- [ ] Upload successful (no timeout errors)
- [ ] Serial monitor shows startup messages
- [ ] Display shows initialization splash screen
- [ ] WiFi scanning begins (see in Serial)
- [ ] WiFi connects to "Itzz_Infinity" network
- [ ] Time synchronizes via NTP
- [ ] Display shows correct current time
- [ ] Time updates every second (watch seconds increment)
- [ ] Date shows correct day/month/year
- [ ] Network info displays with IP address
- [ ] WiFi signal strength (RSSI) displays correctly
- [ ] Temporarily disconnect WiFi to test error handling
- [ ] WiFi reconnects automatically when available

---

## Next Steps

Once Task 2 is working:
- **Task 3:** Integrate DHT11 temperature/humidity sensor
- **Task 4:** Add SD card logging with timestamp
- **Task 5:** Implement web server for CSV download

---

## FAQ

**Q: Can I use 5GHz WiFi?**
A: No, ESP32 only supports 2.4GHz WiFi. Use 2.4GHz band on your router.

**Q: What if multiple networks are available?**
A: System connects to the first available network from your WIFI_NETWORKS array (in order).

**Q: Can I hide WiFi password in code?**
A: Not truly, as compiled binary can be reverse-engineered. Use secure networks or store in separate partition.

**Q: Does it work offline?**
A: Yes, after initial time sync. System keeps time via internal RTC. Without internet, time will drift slowly.

**Q: How often does it re-sync time?**
A: Currently on-connect only. Can be extended with periodic re-sync if needed.

---

## Support

Check Serial Monitor output for detailed diagnostic messages. Enable verbose logging by searching for `Serial.println()` statements in the code.
