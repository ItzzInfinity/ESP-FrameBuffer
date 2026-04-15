# Task 3.1: DHT11 Sensor + Internet Time (Partial Updates)

## What It Does

Advanced Task 3 with intelligent updates:
- ✅ Displays **time** (synced via NTP, updates every 60 seconds)
- ✅ Displays **temperature** from DHT11 (reads every 2 seconds)
- ✅ Displays **humidity** from DHT11 (reads every 2 seconds)
- ✅ **Partial screen updates** - Only refreshes changed regions (no flicker)
- ✅ **Smart change detection** - Only updates display if values changed
- ✅ **Minimal refreshes** - Reduces unnecessary screen updates
- ✅ Time has **no seconds** (updates every 60 seconds for stability)

## Display Output

```
Internet Time
14:32
──────────────────────────
Temperature
  25.3 C

──────────────────────────
Humidity
  65 %
```

## Screen Regions (Partial Updates)

| Region | Height | Content | Update Trigger |
|--------|--------|---------|-----------------|
| Time | 35px | Clock (HH:MM) | Every 60 sec |
| Temperature | 60px | Temp reading | When changed ±0.5°C |
| Humidity | 65px | Humidity reading | When changed ±2.0% |

## Key Innovation: Smart Change Detection

```cpp
// Only updates if SIGNIFICANT change (avoids noise)
if (abs(temperature - prev_temperature) >= 0.5) {  // ±0.5°C
  updateTempDisplay(temperature);
}

if (abs(humidity - prev_humidity) >= 2.0) {  // ±2.0%
  updateHumidDisplay(humidity);
}
```

**Result:** Display updates smoothly without flickering, even with sensor noise.

## Setup (5 Minutes)

### 1. Install Adafruit DHT Library
```
Arduino IDE:
  Sketch → Include Library → Manage Libraries
  Search: "DHT sensor library by Adafruit"
  Click Install
```

### 2. Wire DHT11
```
DHT11 VCC → 3.3V
DHT11 DATA → GPIO 14
DHT11 GND → GND
(Optional: 10K pull-up resistor between DATA and 3.3V)
```

### 3. Set WiFi Credentials (if different)
```cpp
Line 44-45:
#define SSID     "Itzz_Infinity"
#define PASSWORD "123456789"
```

### 4. Upload

```
Arduino IDE:
  Select Board: ESP32 or ESP32-C3
  Select Port: Your USB port
  Upload
```

## Serial Monitor Output

```
Task 3.1: DHT11 Sensor + Internet Time
Display initialized
DHT11 initialized on GPIO 14
Connecting to WiFi: Itzz_Infinity
.....✓ WiFi Connected
Syncing time via NTP...
.....✓ Time synced: Thu Apr 17 14:32:45 2026
WiFi disconnected (using internal clock)

Time updated: 14:32
Temp: 25.3°C, Humidity: 65%
Temp: 25.1°C, Humidity: 65%  (no display update - change < 0.5°C)
Temp: 25.8°C, Humidity: 65%  (display updates - change = 0.5°C)
```

## Code Features

### Time Management
- **NTP Sync**: Fetches time at startup
- **WiFi Disconnect**: Turns off WiFi after sync (saves power)
- **Internal Clock**: Uses accurate internal RTC after sync
- **Update Interval**: Every 60 seconds (stable, prevents flicker)
- **Format**: HH:MM (no seconds)

### Sensor Management
- **Read Interval**: Every 2 seconds (DHT11 minimum)
- **Change Detection**: Compares with previous value
- **Temperature Sensitivity**: Updates if change ≥ 0.5°C
- **Humidity Sensitivity**: Updates if change ≥ 2.0%
- **Noise Filtering**: Prevents small fluctuations from updating display

### Display Management
- **Partial Updates**: Only redraws changed regions
- **Region 1 (Time)**: 35px height, updates every 60s
- **Region 2 (Temp)**: 60px height, updates on change
- **Region 3 (Humidity)**: 65px height, updates on change
- **Zero Flicker**: Each region updates independently

## Configuration

### Change WiFi Credentials
```cpp
Line 44-45:
#define SSID     "your_ssid"
#define PASSWORD "your_password"
```

### Change Timezone
```cpp
Line 46:
#define GMT_OFFSET 19800  // UTC+5:30 (in seconds)
// UTC+0 = 0
// UTC-5 = -18000
```

### Change DHT11 Pin
```cpp
Line 49:
#define DHT_PIN 14  // Change to your GPIO
```

### Change Change Detection Sensitivity
```cpp
Line 106-107:
bool temp_changed = (abs(temperature - prev_temperature) >= 0.5);   // ← Temp tolerance
bool humid_changed = (abs(humidity - prev_humidity) >= 2.0);        // ← Humidity tolerance
```

Lower values = more updates, higher values = fewer updates

## Timing Explanation

### DHT11 Reads Every 2 Seconds
```
0s   : Read temp & humidity
2s   : Read temp & humidity (check if changed)
4s   : Read temp & humidity (check if changed)
...etc
```

### Time Updates Every 60 Seconds
```
0s   : Display initial time (14:32)
60s  : Update time display (14:33)
120s : Update time display (14:34)
...etc
```

**Why every 60 seconds?**
- DHT11 reads every 2 seconds (frequent updates)
- Time doesn't need second hand (no seconds shown)
- Updating every 60 seconds prevents display flicker
- Reduces CPU load

## Performance

### CPU Usage
```
DHT reading:    ~50ms (every 2 seconds)
Time check:     ~5ms (every 60 seconds)
Display update: ~20-50ms (only if changed)

Average: ~2-5% CPU during normal operation
```

### Power Consumption
```
Startup (WiFi sync):  ~120mA for ~3 seconds
Normal operation:     ~20-30mA
Peak (display update):~40-50mA

Battery (500mAh):     ~20+ hours continuous
```

## Display Information Hierarchy

1. **Time** (Top)
   - Most important reference
   - Updates stably every minute
   - Large, clear format

2. **Temperature** (Middle)
   - Updated when changes
   - Large, easy to read
   - Shows decimal precision

3. **Humidity** (Bottom)
   - Updated when changes
   - Complements temperature
   - Integer format (no decimals)

## Testing Checklist

After upload:
- [ ] Serial shows time sync successful
- [ ] Display shows clock (HH:MM format)
- [ ] Temperature displays correctly
- [ ] Humidity displays correctly
- [ ] Time updates every 60 seconds
- [ ] Temperature updates when changed ±0.5°C or more
- [ ] Humidity updates when changed ±2.0% or more
- [ ] No flickering on display
- [ ] No "Sensor Error" messages

## Comparison: Task 3 vs Task 3.1

| Feature | Task 3 | Task 3.1 |
|---------|--------|----------|
| **Display** | Temp + Humidity only | Time + Temp + Humidity |
| **Updates** | Every 2s (always) | Partial + Smart change detection |
| **Time** | None | Internet time (no seconds) |
| **Flicker** | Minimal | Zero (intelligent updates) |
| **Lines** | ~170 | ~280 |
| **Complexity** | Simple | Advanced |

## Troubleshooting

| Problem | Solution |
|---------|----------|
| **WiFi connection fails** | Check SSID/password, verify WiFi network available |
| **Time shows wrong value** | Check GMT_OFFSET value, verify NTP server reachable |
| **Sensor shows error** | Check DHT11 wiring, try pull-up resistor, verify GPIO pin |
| **Display flickering** | Normal - reduce change detection sensitivity if too frequent |
| **No time displayed** | Verify WiFi connection worked during startup |

## Power Optimization Tips

1. **Reduce DHT read frequency** (if not needed every 2 seconds)
   ```cpp
   #define DHT_READ_INTERVAL 5000  // 5 seconds instead of 2
   ```

2. **Increase time update interval** (if 60 seconds is too frequent)
   ```cpp
   #define TIME_UPDATE_INTERVAL 300000  // 5 minutes instead of 1
   ```

3. **Increase change detection tolerance** (fewer display updates)
   ```cpp
   bool temp_changed = (abs(temperature - prev_temperature) >= 1.0);   // Was 0.5
   bool humid_changed = (abs(humidity - prev_humidity) >= 5.0);        // Was 2.0
   ```

## Code Quality

- ✅ Modular functions
- ✅ Clear region definitions
- ✅ Intelligent change detection
- ✅ Partial screen updates
- ✅ Proper error handling
- ✅ Comprehensive comments
- ✅ Minimal code (~280 lines)

---

**Ready to test?** Upload `task3_1_DHT11WithTime.ino` and monitor serial output!
