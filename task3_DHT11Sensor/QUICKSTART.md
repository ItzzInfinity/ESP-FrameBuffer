# Task 3: Quick Setup Guide

## What You Need

1. **DHT11 Sensor** - Any DHT11 module (usually comes with 3-pin connector)
2. **10K Resistor** - Pull-up resistor (optional but recommended)
3. **Arduino IDE** - With DHT library installed

## Step 1: Install Library (1 minute)

Arduino IDE menu:
```
Sketch → Include Library → Manage Libraries
```

Search: `DHT sensor library by Adafruit`

Click: **Install**

## Step 2: Wire DHT11 (2 minutes)

```
DHT11 Module    →    ESP32 / ESP32-C3
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
VCC (+)         →    3.3V
DATA (out)      →    GPIO 14
GND (-)         →    GND
```

**Optional: Add 10K resistor**
- Between DATA pin and 3.3V (improves signal stability)

## Step 3: Upload Code (3 minutes)

1. Copy code from `task3_DHT11Sensor.ino`
2. Paste in Arduino IDE
3. Select board: **ESP32** or **ESP32-C3**
4. Select COM port
5. Click **Upload**

## Step 4: Verify (30 seconds)

Open Serial Monitor (115200 baud):
```
Expected output:
Task 3: DHT11 Sensor Display
Display initialized
DHT11 initialized on GPIO 14
Temperature: 25.3°C, Humidity: 65%
```

Check TFT display shows:
- Reading temperature & humidity
- Values update every 2 seconds
- No "Sensor Error" message

## Done! ✓

Display should show:
```
DHT11 SENSOR
──────────────────────
Temperature
   25.3 C
──────────────────────
Humidity
   65 %
```

---

## If Something's Wrong

**Display shows "Sensor Error":**
1. Check GPIO 14 wiring
2. Add 10K pull-up resistor
3. Try different GPIO pin

**No Serial output:**
1. Check USB cable
2. Verify COM port selected
3. Baud rate must be 115200

**Values seem wrong:**
1. DHT11 not good for <15% or >90% humidity
2. Keep away from direct heat
3. Normal range is 0-50°C

---

## Code Modification Cheat Sheet

### Change DHT Pin
```cpp
Line 28:
#define DHT_PIN 14  // Change number
```

### Change Fahrenheit
```cpp
Line 91:
// Replace:
temperature = dht.readTemperature();
// With:
temperature = dht.readTemperature(true);
```

### Slow Down Updates
```cpp
Line 53:
#define READ_INTERVAL 2000  // Change to 5000 for 5 seconds
```

---

**Need help?** Check README.md for detailed troubleshooting.
