# Task 3.1: Quick Start (5 Minutes)

## Install & Wire (2 Minutes)

### 1. Library Installation
```
Arduino IDE → Sketch → Include Library → Manage Libraries
Search: "DHT sensor library by Adafruit"
Click: Install
```

### 2. DHT11 Wiring
```
DHT11 VCC  → 3.3V
DHT11 DATA → GPIO 14  
DHT11 GND  → GND
```

## Upload Code (3 Minutes)

1. **Copy code** from `task3_1_DHT11WithTime.ino`
2. **Open Arduino IDE**
3. **Select Board**: ESP32 or ESP32-C3
4. **Select Port**: Your USB port
5. **Upload**

## Verify (30 Seconds)

Open Serial Monitor (115200 baud):
```
Expected:
✓ WiFi Connected
✓ Time synced: Thu Apr 17 14:32:45 2026
WiFi disconnected
```

Check TFT Display:
```
Internet Time
14:32
──────────────────────
Temperature
  25.3 C
──────────────────────
Humidity
  65 %
```

## Done! ✓

Display shows:
- Time (updates every 60 seconds)
- Temperature (updates when changes ±0.5°C)
- Humidity (updates when changes ±2.0%)
- Zero flicker (partial updates only)

---

## Edit WiFi (if needed)
```cpp
Line 44-45:
#define SSID     "your_ssid"
#define PASSWORD "your_password"
```

## Edit Timezone (if needed)
```cpp
Line 46:
#define GMT_OFFSET 19800  // UTC+5:30
// Change to your timezone
```

---

**See README.md for full configuration options & troubleshooting**
