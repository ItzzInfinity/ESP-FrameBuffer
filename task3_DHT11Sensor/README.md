# Task 3: DHT11 Temperature & Humidity Sensor

## What It Does

- ✅ Reads temperature from DHT11 sensor
- ✅ Reads humidity from DHT11 sensor
- ✅ Displays both values on TFT LCD
- ✅ Updates every 2 seconds (DHT11 minimum)
- ✅ Shows sensor errors if reading fails
- ✅ Clean, simple display formatting

## Display Output

```
  DHT11 SENSOR
──────────────────────────
Temperature
   25.3 C

──────────────────────────
Humidity
   65 %

Updates every 2s
```

## Setup (5 Minutes)

### 1. Install DHT Library

**Arduino IDE:**
```
Sketch → Include Library → Manage Libraries
Search: "DHT sensor library by Adafruit"
Click Install
```

### 2. Connect DHT11

| DHT11 Pin | Connection |
|-----------|-----------|
| Pin 1 (VCC) | 3.3V |
| Pin 2 (DATA) | GPIO 14 (can change below) |
| Pin 3 (NC) | Not connected |
| Pin 4 (GND) | GND |

**Optional Pull-up Resistor:**
- Add 10K resistor between DATA pin and VCC (improves reliability)

### 3. Check Pin Configuration

```cpp
Line 28:
#define DHT_PIN 14  // GPIO 14 for both ESP32 and ESP32-C3
```

**Common GPIO options:**
- ESP32: GPIO 14, 12, 13, 26, 27 (avoid 6,7,8,9,10,11 - flash)
- ESP32-C3: GPIO 0-5, 9-10

### 4. Upload Code

```
Arduino IDE:
  1. Select Board: ESP32 or ESP32-C3
  2. Select Port: Your USB port
  3. Upload
```

## Serial Monitor Output

```
Task 3: DHT11 Sensor Display
Display initialized
DHT11 initialized on GPIO 14

Temperature: 25.3°C, Humidity: 65%
Temperature: 25.4°C, Humidity: 64%
Temperature: 25.3°C, Humidity: 65%
```

Updates every 2 seconds.

## Code Features

### Sensor Reading
- **Type**: DHT11 (temperature + humidity)
- **Read Interval**: 2 seconds (DHT11 specification minimum)
- **Accuracy**: ±2°C, ±5% RH
- **Range**: 0-50°C, 20-90% RH

### Display
- **Temperature**: Large green text, 1 decimal place (25.3°C)
- **Humidity**: Large orange text, 0 decimal places (65%)
- **Error Handling**: Shows "Sensor Error!" if reading fails
- **Update**: Every 2 seconds

### Reliability
```cpp
// Validates reading before display
if (isnan(humidity) || isnan(temperature)) {
  // Display error message
} else {
  // Display valid sensor data
}
```

## Configuration Options

### Change Read Interval

```cpp
Line 53:
#define READ_INTERVAL 2000  // milliseconds
// Change to 5000 for 5 seconds
// Change to 10000 for 10 seconds
```

**Important**: DHT11 minimum is ~2 seconds between reads.

### Change DHT Pin

```cpp
Line 28:
#define DHT_PIN 14  // Change to your GPIO
```

### Change Temperature Unit to Fahrenheit

```cpp
// In readSensor() function, replace:
temperature = dht.readTemperature();

// With:
temperature = dht.readTemperature(true);  // true = Fahrenheit
```

Then update display units from "C" to "F".

## Troubleshooting

| Problem | Solution |
|---------|----------|
| **Sensor Error shown** | Check DHT11 wiring, verify pin number, try pull-up resistor |
| **Readings way off** | DHT11 may be faulty, try different GPIO pin |
| **Display blank** | Check TFT wiring (same as Task 1) |
| **Not reading** | Verify DHT library installed, check COM port |
| **Erratic values** | Add 10K pull-up resistor between DATA and VCC |

## Hardware Comparison

| Sensor | Temp Range | Humidity Range | Accuracy | Min Read |
|--------|-----------|---------------|----------|----------|
| DHT11 | 0-50°C | 20-90% | ±2°C/5% | 2 seconds |
| DHT22 | -40 to 80°C | 0-100% | ±0.5°C/2% | 2 seconds |
| BME280 | -40 to 85°C | 0-100% | ±1°C/3% | <1 second |

(DHT11 used per project specifications)

## DHT11 Pinout Details

```
       ┌─────────────┐
       │ DHT11 Front│
       │             │
  VCC  │ 1   [ ]   4 │ GND
  DATA │ 2   [ ]   3 │ NC
       └─────────────┘
```

**Pin Configuration:**
- Pin 1: VCC (3.0-5.5V, typically 3.3V)
- Pin 2: DATA (serial output)
- Pin 3: NC (not connected)
- Pin 4: GND (ground)

## Code Size

- **Lines**: ~170
- **Compiled**: ~180 KB
- **RAM**: ~50 KB
- **Libraries**: Adafruit_DHT, Adafruit_ST7735, Adafruit_GFX

## Reading Frequency

DHT11 protocol timing:
```
Total read time: ~500ms per sensor read
Minimum between reads: 2 seconds
Our interval: 2 seconds (optimal)
```

If you read too fast (< 2 seconds), DHT returns -1 (error).

## Power Consumption

- **Idle**: ~20-30mA
- **Sensor active**: ~30-40mA
- **TFT display**: ~20-40mA (depends on brightness)

**Total**: ~50-80mA typical operation

## Testing Checklist

After upload:
- [ ] Serial monitor shows "DHT11 initialized"
- [ ] Readings appear every 2 seconds in Serial
- [ ] Display shows "DHT11 SENSOR" header
- [ ] Temperature displays (large green text)
- [ ] Humidity displays (large orange text)
- [ ] Values change as room conditions change
- [ ] No "Sensor Error" messages
- [ ] Display updates smoothly every 2 seconds

## Display Information Hierarchy

1. **Header**: "DHT11 SENSOR" (identification)
2. **Temperature**: Large, prominent (primary measurement)
3. **Humidity**: Large, prominent (secondary measurement)
4. **Units**: Clear (°C and %)
5. **Status**: "Updates every 2s" (bottom info)

## Common Issues & Fixes

**Issue**: Always shows "Sensor Error"
```
Solutions:
1. Check GPIO pin number matches #define DHT_PIN
2. Verify wiring (VCC, DATA, GND connections)
3. Try pull-up resistor (10K between DATA and VCC)
4. Test with different GPIO pin
5. Check DHT11 is not damaged
```

**Issue**: Readings stuck at same value
```
Solutions:
1. Increase READ_INTERVAL (currently 2000ms)
2. Check DHT11 is getting new data, not cached
3. Move DHT11 away from heat sources
```

**Issue**: Temperature way too high/low
```
Solutions:
1. Verify DHT11 is not in direct sunlight
2. Check for heat sources near sensor
3. Verify GPIO pin not electrically noisy
4. Try adding pull-up resistor
```

## Next Steps (Task 4)

Task 4 will add:
- SD card storage
- CSV logging of temperature/humidity
- Timestamp record
- Periodic saving (every 5-10 minutes)

The sensor reading code from Task 3 will be reused directly in Task 4.

## Minimal Design Philosophy

✓ Only essential features
✓ Clean display layout
✓ Proper error handling
✓ Efficient sensor reading
✓ Minimal code (~170 lines)
✓ Clear comments
✓ Easy to understand

---

**Ready to test?** Copy code and upload!
