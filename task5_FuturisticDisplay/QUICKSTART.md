# Task 5: Quick Start (5 Minutes)

## Wire DHT11

```
DHT11 VCC  → 3.3V
DHT11 DATA → GPIO 14
DHT11 GND  → GND
```

## Install Library

Arduino IDE:
```
Sketch → Include Library → Manage Libraries
Search: "DHT sensor library by Adafruit"
Click: Install
```

## Upload

1. Copy code from `task5_FuturisticDisplay.ino`
2. Select Board: ESP32 or ESP32-C3
3. Select Port: Your USB
4. Upload

## Check Display

Should show:
```
T 25.3 °C
(Color: Green = Comfortable)

H 65%
(Color: Cyan)

F 28.5 °C
(Color: Dynamic based on feels-like)
```

### Icons Explained

```
T → Temperature (color changes with temperature)
H → Humidity (always cyan)
F → Feels-Like (color changes with felt temperature)
```

### Colors Explained

```
BLUE   → Cold (< 18°C)
GREEN  → Comfortable (18-24°C)
ORANGE → Warm (24-30°C)
RED    → Hot (> 30°C)
```

## Done! ✓

No WiFi, no SD card - just clean sensor display with equal-sized digits.

Check Serial Monitor for values updating.

---

See README.md for customization options.
