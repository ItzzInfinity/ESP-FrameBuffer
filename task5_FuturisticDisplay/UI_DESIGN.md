# Task 5: Cyberpunk HUD UI Design

## Overview

Minimal, geometric cyberpunk dashboard for embedded 160×128 display. Built entirely from:
- Lines (`drawLine`)
- Rectangles (`drawRect`, `fillRect`)
- Text (`setTextColor`, `setTextSize`, `print`)

No images, no gradients, no shadows — pure technical minimalism.

---

## Screen Layout (Pixel Coordinates)

```
160 pixels wide × 128 pixels tall (landscape)

┌──────────────────────────────────────────────────────────┐
│ REGION 1: TEMPERATURE (y: 0-42)                          │
│ ┌────────────────────────────────────────────────────────┤
│ │ ✓ Icon Area    ✓ Value Area (large)    ✓ Unit Label    │
│ └────────────────────────────────────────────────────────┤
├──────────────────────────────────────────────────────────┤
│ REGION 2: HUMIDITY (y: 42-84)                            │
│ ┌────────────────────────────────────────────────────────┤
│ │ ✓ Icon Area    ✓ Value Area (large)    ✓ Unit Label    │
│ └────────────────────────────────────────────────────────┤
├──────────────────────────────────────────────────────────┤
│ REGION 3: FEELS LIKE (y: 84-128)                         │
│ ┌────────────────────────────────────────────────────────┤
│ │ ✓ Icon Area    ✓ Value Area (large)    ✓ Unit Label    │
│ └────────────────────────────────────────────────────────┘
```

---

## Region Structure (Per Section: 42 pixels tall)

Each region follows identical structure:

```
REGION HEIGHT: 42 pixels
MARGIN: 5 pixels (all sides)
USABLE HEIGHT: 32 pixels

┌─ x:0                                              x:160 ─┐
│                                                           │
│ y:0   [Left Border: 1px] ┌────────────────────────────┐  │
│       │                   │                            │  │
│ y:5   │  [ICON]    [VALUE]    [UNIT]                  │  │
│       │  (15×15px) (80px wide) (20px)                 │  │
│       │                   │                            │  │
│ y:37  │                   └────────────────────────────┘  │
│       [Thin divider line: 1px] (separates regions)       │
│                                                           │
└─ y:42 ──────────────────────────────────────────────────┘

Coordinates within region:
- LEFT BORDER: x=1, y=0 to y=42 (1px vertical line)
- ICON: x=10-24, y=8-22 (15×15px area, centered)
- VALUE: x=35-115, y=5-35 (large text, centered vertically)
- UNIT: x=125-155, y=8-20 (small text label)
- BOTTOM BORDER: y=41, x=1 to x=158 (thin divider line)
```

---

## Colors (RGB565 Hex)

```cpp
// Background & Borders
#define COLOR_BLACK        0x0000   // Pure black
#define COLOR_DARK_BLUE    0x0010   // Very subtle background tint
#define COLOR_GRID         0x0410   // Subtle grid lines (if used)

// Temperature Colors (Dynamic - based on value)
#define COLOR_COLD         0x001F   // Deep blue     (< 15°C)
#define COLOR_COOL         0x041F   // Blue          (15-18°C)
#define COLOR_COMFORT      0x07E0   // Neon green    (18-24°C)
#define COLOR_WARM         0xFE00   // Neon orange   (24-30°C)
#define COLOR_HOT          0xF800   // Bright red    (> 30°C)

// Humidity (Always same accent)
#define COLOR_HUMIDITY     0x07FF   // Cyan (neon water)

// Accents & Secondary Elements
#define COLOR_ACCENT_MAG   0xF81F   // Magenta accent (optional)
#define COLOR_GRID_ACCENT  0x0F1F   // Purple grid (optional)
```

---

## Section 1: TEMPERATURE (y: 0-42)

### Icon Design (15×15px box, x: 10-24, y: 8-22)

**Thermometer** — Simple geometric:
```
        [1px frame at x:10, y:8, size 15×15]
        ┌─────────┐
        │    ▀▀   │  ← Bulb (top, 2 rows)
        │    ║    │  ← Stem (center, hollow)
        │    ║    │  
        │    ║    │  
        │    ━━   │  ← Filled block (mercury)
        │    ━━   │
        │    ▄▄   │  ← Base
        └─────────┘

Draw:
  drawRect(10, 8, 15, 15, tempColor)     // Frame
  fillRect(11, 9, 13, 2, tempColor)      // Bulb (top)
  drawLine(17, 11, 17, 18, tempColor)    // Stem
  fillRect(11, 19, 13, 3, tempColor)     // Mercury level
```

### Value Display

**Position:** x: 35-115, y: 5-35  
**Font Size:** 3 (largest, ~24px tall)  
**Color:** Dynamic (temp-based)  
**Content:** `28.5` (no degree symbol in this area)

```cpp
tft.setCursor(35, 5);
tft.setTextColor(getTempColor(temp));
tft.setTextSize(3);
tft.printf("%.1f", temperature);
```

### Unit Label

**Position:** x: 125, y: 8  
**Font Size:** 2 (~16px)  
**Color:** Same as value  
**Content:** `°C`

```cpp
tft.setCursor(125, 8);
tft.setTextColor(getTempColor(temp));
tft.setTextSize(2);
tft.print("C");
```

### Borders & Dividers

```cpp
// Left border (accent line)
tft.drawLine(2, 0, 2, 42, tempColor);

// Bottom divider (separator to next region)
tft.drawLine(0, 41, 159, 41, COLOR_GRID);

// Optional: Corner accent (top-left)
tft.drawLine(1, 1, 6, 1, tempColor);  // Horizontal
tft.drawLine(1, 1, 1, 5, tempColor);  // Vertical
```

---

## Section 2: HUMIDITY (y: 42-84)

### Icon Design (15×15px box, x: 10, y: 50)

**Water Droplet** — Simple geometric:
```
        ┌─────────┐
        │   ∆∆    │  ← Top point
        │  ∆▓∆    │  
        │ ∆▓▓▓∆   │  ← Filled diamond/drop
        │  ▓▓▓    │
        │   ▓▓    │  ← Taper to point
        │    ▓    │
        └─────────┘

Draw:
  drawRect(10, 50, 15, 15, COLOR_HUMIDITY)      // Frame
  drawLine(17, 52, 20, 56, COLOR_HUMIDITY)      // Left edge
  drawLine(20, 56, 17, 60, COLOR_HUMIDITY)      // Right edge
  drawLine(17, 60, 17, 62, COLOR_HUMIDITY)      // Bottom point
  fillRect(13, 54, 9, 8, COLOR_HUMIDITY)        // Fill (simplified)
```

### Value Display

**Position:** x: 35, y: 47  
**Font Size:** 3 (largest)  
**Color:** Cyan (COLOR_HUMIDITY)  
**Content:** `65`

```cpp
tft.setCursor(35, 47);
tft.setTextColor(COLOR_HUMIDITY);
tft.setTextSize(3);
tft.printf("%.0f", humidity);
```

### Unit Label

**Position:** x: 130, y: 50  
**Font Size:** 2  
**Color:** Cyan  
**Content:** `%`

```cpp
tft.setCursor(130, 50);
tft.setTextColor(COLOR_HUMIDITY);
tft.setTextSize(2);
tft.print("%");
```

### Borders & Dividers

```cpp
// Left border
tft.drawLine(2, 42, 2, 84, COLOR_HUMIDITY);

// Bottom divider
tft.drawLine(0, 83, 159, 83, COLOR_GRID);

// Corner accent (top-left, within this section)
tft.drawLine(1, 43, 6, 43, COLOR_HUMIDITY);
tft.drawLine(1, 43, 1, 48, COLOR_HUMIDITY);
```

---

## Section 3: FEELS LIKE (y: 84-128)

### Icon Design (15×15px box, x: 10, y: 92)

**Heat/Energy** — Simple geometric (pulse/star):
```
        ┌─────────┐
        │    ★    │  ← Center point
        │   ★★★   │  ← Star points
        │  ★★★★★  │
        │   ★★★   │  ← Radiating out
        │    ★    │
        └─────────┘

Draw:
  drawRect(10, 92, 15, 15, feelsLikeColor)      // Frame
  // Center cross
  drawLine(17, 92, 17, 107, feelsLikeColor)     // Vertical
  drawLine(10, 99, 25, 99, feelsLikeColor)      // Horizontal
  // Diagonal points
  drawLine(12, 94, 22, 105, feelsLikeColor)
  drawLine(22, 94, 12, 105, feelsLikeColor)
```

### Value Display

**Position:** x: 35, y: 89  
**Font Size:** 3 (largest)  
**Color:** Dynamic (feels-like temp color)  
**Content:** `31.5`

```cpp
tft.setCursor(35, 89);
tft.setTextColor(getTempColor(feels_like));
tft.setTextSize(3);
tft.printf("%.1f", feels_like);
```

### Unit Label

**Position:** x: 125, y: 92  
**Font Size:** 2  
**Color:** Dynamic (feels-like color)  
**Content:** `°C`

```cpp
tft.setCursor(125, 92);
tft.setTextColor(getTempColor(feels_like));
tft.setTextSize(2);
tft.print("C");
```

### Borders & Dividers

```cpp
// Left border
tft.drawLine(2, 84, 2, 128, feelsLikeColor);

// Bottom border (screen edge)
tft.drawLine(0, 127, 159, 127, COLOR_GRID);

// Corner accents (both top-left and bottom-left)
tft.drawLine(1, 85, 6, 85, feelsLikeColor);
tft.drawLine(1, 85, 1, 90, feelsLikeColor);
```

---

## Optional: Subtle HUD Elements

Add these for more cyberpunk feel (but keep minimal):

### Grid Reference Lines (Very Subtle)

```cpp
// Vertical grid lines (every 40 pixels)
for (int x = 40; x < 160; x += 40) {
  drawLine(x, 0, x, 128, COLOR_GRID);  // 1px opacity, use sparingly
}

// Or: Horizontal scanning lines (simulate CRT)
// for (int y = 0; y < 128; y += 2) {
//   drawLine(0, y, 159, y, COLOR_GRID_SUBTLE);  // Very faint
// }
```

### Corner Accents (Screen Corners)

```cpp
// Top-left corner
tft.drawLine(0, 0, 8, 0, COLOR_ACCENT_MAG);    // Top edge
tft.drawLine(0, 0, 0, 8, COLOR_ACCENT_MAG);    // Left edge

// Top-right corner
tft.drawLine(159, 0, 151, 0, COLOR_ACCENT_MAG);
tft.drawLine(159, 0, 159, 8, COLOR_ACCENT_MAG);

// Bottom-left corner
tft.drawLine(0, 127, 8, 127, COLOR_ACCENT_MAG);
tft.drawLine(0, 127, 0, 120, COLOR_ACCENT_MAG);

// Bottom-right corner
tft.drawLine(159, 127, 151, 127, COLOR_ACCENT_MAG);
tft.drawLine(159, 127, 159, 120, COLOR_ACCENT_MAG);
```

---

## Implementation Checklist

### Initialization Phase (`drawInitialUI()`)

- [x] Fill screen black (`tft.fillScreen(COLOR_BLACK)`)
- [x] Draw 3 section borders (left border + dividers)
- [x] Draw 3 corner accents
- [x] Draw 3 icons (thermometer, droplet, star)
- [x] Optional: Draw subtle grid lines

### Update Phase (per section)

**Temperature:**
- Clear region (fillRect 0, 0, 160, 42, BLACK)
- Redraw border and icon
- Update value (cursor + printf with temp-based color)
- Update unit label

**Humidity:**
- Clear region (fillRect 0, 42, 160, 42, BLACK)
- Redraw border and icon
- Update value (cyan color, no change needed)
- Update unit label

**Feels Like:**
- Clear region (fillRect 0, 84, 160, 44, BLACK)
- Redraw border and icon
- Update value (feels-like color-based)
- Update unit label

---

## Color Temperature Mapping

```cpp
uint16_t getTempColor(float temp) {
  if (temp < 15) return COLOR_COLD;        // Deep blue
  if (temp < 18) return COLOR_COOL;        // Blue
  if (temp < 24) return COLOR_COMFORT;     // Green
  if (temp < 30) return COLOR_WARM;        // Orange
  return COLOR_HOT;                        // Red
}
```

---

## Final Visual Summary

```
┌─────────────────────────────────────────┐
│ ∆  ┌─ Left border (temp color)          │
│    │                                     │
│    │◇◇  28.5  °C   [ICON][VALUE][UNIT]  │
│    │                                     │
│    │                                     │  ← Thin divider (grid color)
│    ├─────────────────────────────────────┤
│ ∆  │                                     │
│    │◇◇◇ 65   %     [ICON][VALUE][UNIT]  │
│    │◇◇                                   │
│    │                                     │  ← Thin divider
│    ├─────────────────────────────────────┤
│ ∆  │                                     │
│    │★★★ 31.5 °C   [ICON][VALUE][UNIT]  │
│    │ ★                                   │
│    │                                     │
└─────────────────────────────────────────┘

Legend:
∆  = Corner accent (small L-shape)
◇◇ = Droplet icon
★★ = Heat/star icon
```

---

## Customization Options

### Reduce Complexity
- Remove corner accents (if space is tight)
- Remove grid lines
- Remove left border (use only dividers)

### Increase Cyberpunk Vibe
- Add more corner accents (all 4 corners)
- Add subtle scanning lines
- Change divider color to magenta
- Add "ID: TEMP", "ID: HUM", "ID: FEEL" labels above each section

### Increase Readability
- Increase section height to 48px
- Use text size 4 for values (larger digits)
- Add more padding around values

---

## Summary

**What Makes This Work:**
1. ✅ **Geometric icons** (no images needed)
2. ✅ **Consistent spacing** (5px margins throughout)
3. ✅ **Clear hierarchy** (large values, small labels)
4. ✅ **Minimal lines** (borders + dividers only)
5. ✅ **High contrast** (bright colors on black)
6. ✅ **Dynamic colors** (temperature-based feedback)
7. ✅ **CRT/HUD inspired** (corner accents, thin lines)
8. ✅ **Partial updates** (update only changed regions)

**Drawing Primitives Used:**
- `drawLine(x1, y1, x2, y2, color)` — borders, dividers, icon details
- `drawRect(x, y, w, h, color)` — section frames, icon boxes
- `fillRect(x, y, w, h, color)` — background fills, icon solid areas
- `setCursor(x, y)` + `print(text)` — labels and values
- `setTextColor(color)` + `setTextSize(size)` — text styling

**Estimated Draw Calls per Update:**
- Initial UI: ~30 draw calls (icons + borders + accents)
- Per temperature update: ~5 draw calls
- Per humidity update: ~5 draw calls
- Per feels-like update: ~5 draw calls

**Total**: Minimal redraws, maximum visual impact.
