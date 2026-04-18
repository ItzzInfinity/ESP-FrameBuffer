# Task 5: Enhanced Dashboard Design

## Overview

Upgrade to a **card-based dashboard** layout inspired by IoT dashboards, but optimized for 160×128 display.

**Key changes:**
- Card-like structure (borders + subtle background)
- More prominent cyan accents
- Removed problematic magenta (replace with cyan)
- Enhanced visual hierarchy
- Subtle grid/pattern elements
- More "dashboard" feel while staying minimal

---

## Color Scheme (Updated)

```cpp
// Background & Structure
#define COLOR_BLACK        0x0000   // Pure black (background)
#define COLOR_CARD_BG      0x0410   // Very dark blue-gray (card background)

// Primary Accents
#define COLOR_CYAN         0x07FF   // Bright cyan (accent lines, borders)
#define COLOR_CYAN_DIM     0x0410   // Dim cyan (subtle elements)

// Temperature Colors (Dynamic - based on value)
#define COLOR_COLD         0x001F   // Deep blue (< 15°C)
#define COLOR_COOL         0x041F   // Blue (15-18°C)
#define COLOR_COMFORT      0x07E0   // Neon green (18-24°C)
#define COLOR_WARM         0xFE00   // Neon orange (24-30°C)
#define COLOR_HOT          0xF800   // Bright red (> 30°C)

// Grid & Details
#define COLOR_GRID         0x0410   // Subtle grid lines
#define COLOR_ACCENT       0x07FF   // Cyan (replaces magenta)
```

**Note:** Removed magenta (0xF81F) because it renders too red on displays. Using cyan instead for all accents.

---

## Enhanced Layout

```
┌─ SCREEN DIMENSIONS: 160×128 ─┐
│                               │
│  ╔════════════════════════╗   │
│  ║ T │        [25.3]      ║   │ ← Card 1: Temperature
│  ║   │ ▲▲▲▲▲▲ COMFORT ▲▲▲▲ ║   │    (with subtle trend indicator)
│  ╚════════════════════════╝   │
│                               │
│  ╔════════════════════════╗   │
│  ║ H │        [65%]       ║   │ ← Card 2: Humidity
│  ║   │ ─────────██████─── ║   │    (with horizontal bar)
│  ╚════════════════════════╝   │
│                               │
│  ╔════════════════════════╗   │
│  ║ F │        [28.5]      ║   │ ← Card 3: Feels-Like
│  ║   │ ▼▼▼▼▼▼ WARM  ▼▼▼▼   ║   │    (with trend indicator)
│  ╚════════════════════════╝   │
│                               │
└───────────────────────────────┘
```

---

## Card Structure (Each Section: 42px tall)

```
CARD FRAME:
  Outer border: 1px cyan at:
    - Top: y = section_start
    - Left: x = 1
    - Bottom: y = section_end - 1
    - Right: x = 159

CARD PADDING:
  5px margin on all sides

CONTENT ZONES:
  ┌─ Icon (x: 5-20, y: centered) ─────────────────────┐
  │                                                    │
  │  Icon Box:    Value Area:          Label/Status:  │
  │  ┌────┐       x: 35-120            x: 130-155    │
  │  │ ▲▲ │       y: 5-35               y: 8-20      │
  │  │ ▲▲ │       "28.5"               "COMFORT"     │
  │  └────┘       (large text)          (small text)  │
  │                                                    │
  └────────────────────────────────────────────────────┘

  BOTTOM INDICATOR (y: 38-40):
    - Temperature: Trend up/down arrow + colored bar
    - Humidity: Horizontal fill bar (0-100%)
    - Feels-Like: Trend indicator + status
```

---

## Updated Icon Designs

### Temperature Icon (Enhanced Thermometer)

```
┌──────────────┐
│   [T]        │
│   ▲▲         │ ← Bulb (filled)
│   ║║         │ ← Stem
│   ║║         │
│   ██         │ ← Mercury level (filled)
│   ██         │
└──────────────┘

Draw Code:
  tft.drawRect(x, y, 15, 15, color);         // Card frame
  tft.fillRect(x+1, y+1, 13, 3, color);      // Bulb
  tft.drawLine(x+7, y+4, x+7, y+12, color);  // Stem left
  tft.drawLine(x+8, y+4, x+8, y+12, color);  // Stem right
  tft.fillRect(x+1, y+13, 13, 2, color);     // Mercury
```

### Humidity Icon (Droplet + Bar)

```
┌──────────────┐
│   [H]        │
│   ◆◆         │ ← Droplet
│  ◆ ◆         │
│   ◆◆         │
│ ▓▓▓▓▓▓▓▓▓▓ ← Horizontal progress bar
│ (represents humidity %)
└──────────────┘

Draw Code:
  tft.drawRect(x, y, 15, 15, color);                // Frame
  // Droplet (simplified)
  tft.drawLine(x+7, y+2, x+4, y+7, color);          // Left edge
  tft.drawLine(x+8, y+2, x+11, y+7, color);         // Right edge
  tft.drawLine(x+4, y+7, x+7, y+13, color);         // Bottom left
  tft.drawLine(x+11, y+7, x+7, y+13, color);        // Bottom right
  tft.fillRect(x+5, y+5, 5, 7, color);              // Fill
  
  // Humidity bar (at bottom of card)
  int barWidth = (humidity / 100.0) * 145;
  tft.drawRect(8, 38, 145, 3, COLOR_CYAN);          // Bar outline
  tft.fillRect(8, 38, barWidth, 3, color);          // Fill based on %
```

### Feels-Like Icon (Enhanced Energy/Star)

```
┌──────────────┐
│   [F]        │
│    ★         │ ← Center point
│   ★★★        │ ← Radiating
│  ★★★★★       │
│   ★★★        │ ← Trend arrows
│    ▼▼        │
└──────────────┘

Draw Code:
  tft.drawRect(x, y, 15, 15, color);           // Frame
  // Center point
  tft.fillRect(x+6, y+6, 3, 3, color);
  // Cross
  tft.drawLine(x+7, y+2, x+7, y+12, color);    // Vertical
  tft.drawLine(x+2, y+7, x+12, y+7, color);    // Horizontal
  // Diagonals
  tft.drawLine(x+4, y+4, x+10, y+10, color);
  tft.drawLine(x+10, y+4, x+4, y+10, color);
```

---

## Card Border Structure

Each card now has:

1. **Top Border (cyan):** Full width, 1px
2. **Left Border (cyan):** Full height, 1px (accent line)
3. **Bottom Border (cyan):** Full width, 1px
4. **Subtle Card Background:** Very dark blue-gray (0x0410) fills the entire card

```cpp
void drawCardFrame(int y_start, int y_end, uint16_t color) {
  // Outer border
  tft.drawLine(0, y_start, 159, y_start, COLOR_CYAN);      // Top
  tft.drawLine(0, y_start, 0, y_end, COLOR_CYAN);          // Left (thin)
  tft.drawLine(0, y_end-1, 159, y_end-1, COLOR_CYAN);      // Bottom
  tft.drawLine(159, y_start, 159, y_end, COLOR_CYAN);      // Right (thin)
  
  // Card content background (very subtle)
  // Optional: tft.fillRect(1, y_start+1, 158, y_end-y_start-2, COLOR_CARD_BG);
}
```

---

## Enhanced Display Updates

### updateTemperatureDisplay()

```cpp
void updateTemperatureDisplay(float temp) {
  uint16_t color = getTempColor(temp);
  
  // Clear region
  tft.fillRect(0, 0, 160, 42, COLOR_BLACK);
  
  // Draw card frame (cyan border)
  drawCardFrame(0, 42, COLOR_CYAN);
  
  // Draw thermometer icon
  drawThermometerIcon(8, 8, color);
  
  // Large temperature value
  tft.setCursor(40, 5);
  tft.setTextColor(color);
  tft.setTextSize(3);
  tft.printf("%.1f", temp);
  
  // Degree + Unit
  tft.setCursor(125, 8);
  tft.setTextColor(color);
  tft.setTextSize(2);
  tft.print("C");
  
  // Status label (COLD, COMFORT, HOT, etc.)
  tft.setCursor(40, 28);
  tft.setTextColor(COLOR_CYAN);
  tft.setTextSize(1);
  if (temp < 15) tft.print("COLD");
  else if (temp < 18) tft.print("COOL");
  else if (temp < 24) tft.print("COMFORT");
  else if (temp < 30) tft.print("WARM");
  else tft.print("HOT!");
  
  // Trend indicator (subtle up/down arrow at bottom)
  if (temp > prev_temperature + 0.5) {
    // Temperature rising
    tft.drawLine(130, 36, 130, 32, COLOR_CYAN);  // Up arrow
    tft.drawLine(127, 35, 130, 32, COLOR_CYAN);
    tft.drawLine(133, 35, 130, 32, COLOR_CYAN);
  } else if (temp < prev_temperature - 0.5) {
    // Temperature falling
    tft.drawLine(130, 32, 130, 36, COLOR_CYAN);  // Down arrow
    tft.drawLine(127, 33, 130, 36, COLOR_CYAN);
    tft.drawLine(133, 33, 130, 36, COLOR_CYAN);
  }
}
```

### updateHumidityDisplay()

```cpp
void updateHumidityDisplay(float humidity) {
  // Clear region
  tft.fillRect(0, 42, 160, 42, COLOR_BLACK);
  
  // Draw card frame (cyan border)
  drawCardFrame(42, 84, COLOR_CYAN);
  
  // Draw droplet icon
  drawWaterDropletIcon(8, 50, COLOR_CYAN);
  
  // Large humidity value
  tft.setCursor(40, 47);
  tft.setTextColor(COLOR_CYAN);
  tft.setTextSize(3);
  tft.printf("%.0f", humidity);
  
  // Percent symbol
  tft.setCursor(130, 50);
  tft.setTextColor(COLOR_CYAN);
  tft.setTextSize(2);
  tft.print("%");
  
  // Horizontal progress bar (humidity level)
  int barWidth = (humidity / 100.0) * 145;
  tft.drawRect(8, 72, 145, 4, COLOR_CYAN);           // Bar outline
  tft.fillRect(8, 72, barWidth, 4, COLOR_CYAN);      // Fill
}
```

### updateFeelsLikeDisplay()

```cpp
void updateFeelsLikeDisplay(float feels_like) {
  uint16_t color = getTempColor(feels_like);
  
  // Clear region
  tft.fillRect(0, 84, 160, 44, COLOR_BLACK);
  
  // Draw card frame (cyan border)
  drawCardFrame(84, 128, COLOR_CYAN);
  
  // Draw heat icon
  drawHeatIcon(8, 92, color);
  
  // Large feels-like value
  tft.setCursor(40, 89);
  tft.setTextColor(color);
  tft.setTextSize(3);
  tft.printf("%.1f", feels_like);
  
  // Degree + Unit
  tft.setCursor(125, 92);
  tft.setTextColor(color);
  tft.setTextSize(2);
  tft.print("C");
  
  // Status (based on feels-like temperature)
  tft.setCursor(40, 110);
  tft.setTextColor(COLOR_CYAN);
  tft.setTextSize(1);
  if (feels_like < 15) tft.print("COLD FEEL");
  else if (feels_like < 18) tft.print("COOL FEEL");
  else if (feels_like < 24) tft.print("COMFY");
  else if (feels_like < 30) tft.print("WARM FEEL");
  else tft.print("HOT FEEL!");
}
```

---

## Visual Result

```
╔════════════════════════════════════════╗
║ ┌──────────────────────────────────┐  ║
║ │ [T]      25.3  °C               │  ║  ← Cyan border (card)
║ │     ▲ COMFORT ▲                 │  ║     Temperature with icon
║ │                                  │  ║     Status + trend
║ └──────────────────────────────────┘  ║
║                                        ║
║ ┌──────────────────────────────────┐  ║
║ │ [H]      65   %                  │  ║  ← Cyan border (card)
║ │     ────────████████────         │  ║     Humidity with icon
║ │                                  │  ║     Progress bar
║ └──────────────────────────────────┘  ║
║                                        ║
║ ┌──────────────────────────────────┐  ║
║ │ [F]      28.5  °C               │  ║  ← Cyan border (card)
║ │     ▼ WARM FEEL ▼                │  ║     Feels-like with icon
║ │                                  │  ║     Status + trend
║ └──────────────────────────────────┘  ║
╚════════════════════════════════════════╝

Legend:
  ═ = Cyan border (accent)
  ─ = Grid/dividers (subtle)
  ▲ = Trend indicator (up)
  ▼ = Trend indicator (down)
  ░░░ = Progress bar fill
```

---

## Color Reference

| Element | Color | Hex | Purpose |
|---------|-------|-----|---------|
| Card Border | Cyan | 0x07FF | Primary accent |
| Background | Black | 0x0000 | Clean minimal |
| Temperature Text | Dynamic | 0x001F-0xF800 | Status feedback |
| Humidity Text | Cyan | 0x07FF | Accent color |
| Feels-Like Text | Dynamic | 0x001F-0xF800 | Status feedback |
| Progress Bar | Cyan | 0x07FF | Visual indicator |
| Status Labels | Cyan | 0x07FF | Information text |
| Trend Arrows | Cyan | 0x07FF | Direction indicator |

---

## Implementation Steps

1. Update color definitions (remove magenta, add COLOR_CARD_BG)
2. Add `drawCardFrame()` function
3. Update `updateTemperatureDisplay()` with status + trend
4. Update `updateHumidityDisplay()` with progress bar
5. Update `updateFeelsLikeDisplay()` with status + trend
6. Keep icon functions as-is (minimal changes)

---

## Dashboard Feel Achieved

✅ **Card-based layout** — Each metric in its own framed box  
✅ **Dark theme** — Pure black background, cyan accents  
✅ **Neon colors** — Bright cyan borders, dynamic temperature colors  
✅ **Data labels** — Status text below values (COMFORT, HOT, etc.)  
✅ **Visual indicators** — Trend arrows, progress bars  
✅ **Minimal geometry** — Only lines and rectangles, no images  
✅ **IoT aesthetic** — Modern dashboard feel on tiny display  

Result: **Cyberpunk IoT dashboard optimized for 160×128 pixels!**
