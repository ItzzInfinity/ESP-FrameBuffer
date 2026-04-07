# Task 5: Implementation Guide for Cyberpunk HUD Design

## Overview

This guide maps the UI design (`UI_DESIGN.md`) to actual code changes in `task5_FuturisticDisplay.ino`.

---

## Step 1: Update Color Definitions

Add these to the color constants section at the top:

```cpp
// === CYBERPUNK HUD COLORS ===
#define COLOR_BLACK        0x0000   // Pure black background
#define COLOR_COLD         0x001F   // Deep blue (< 15°C)
#define COLOR_COOL         0x041F   // Blue (15-18°C)
#define COLOR_COMFORT      0x07E0   // Neon green (18-24°C)
#define COLOR_WARM         0xFE00   // Neon orange (24-30°C)
#define COLOR_HOT          0xF800   // Bright red (> 30°C)
#define COLOR_HUMIDITY     0x07FF   // Cyan
#define COLOR_GRID         0x0410   // Subtle grid/divider color
#define COLOR_ACCENT_MAG   0xF81F   // Magenta (optional corner accents)
```

---

## Step 2: Update `drawInitialUI()` Function

Replace the entire `drawInitialUI()` with this cyberpunk version:

```cpp
void drawInitialUI() {
  // Fill screen with pure black
  tft.fillScreen(COLOR_BLACK);
  
  // ===== DRAW ICONS & SECTION BORDERS =====
  
  // REGION 1: TEMPERATURE (y: 0-42)
  drawThermometerIcon(10, 8, COLOR_COLD);
  drawRegionBorder(0, 0, 42, COLOR_COLD);
  
  // REGION 2: HUMIDITY (y: 42-84)
  drawWaterDropletIcon(10, 50, COLOR_HUMIDITY);
  drawRegionBorder(42, 42, 84, COLOR_HUMIDITY);
  
  // REGION 3: FEELS LIKE (y: 84-128)
  drawHeatIcon(10, 92, COLOR_COLD);  // Will be updated by getTempColor()
  drawRegionBorder(84, 84, 128, COLOR_COLD);
  
  // ===== DRAW CORNER ACCENTS (Optional HUD feel) =====
  drawCornerAccents();
  
  // ===== PLACEHOLDER VALUES =====
  updateTemperatureDisplay(0);
  updateHumidityDisplay(0);
  updateFeelsLikeDisplay(0);
}
```

---

## Step 3: Add Helper Functions for Icons

Add these new functions to draw geometric icons:

```cpp
// ============================================================================
// ICON DRAWING FUNCTIONS
// ============================================================================

void drawThermometerIcon(int x, int y, uint16_t color) {
  // Frame
  tft.drawRect(x, y, 15, 15, color);
  
  // Bulb (top 2 rows of frame)
  tft.fillRect(x + 1, y + 1, 13, 3, color);
  
  // Stem (vertical line down center)
  tft.drawLine(x + 7, y + 4, x + 7, y + 12, color);
  tft.drawLine(x + 8, y + 4, x + 8, y + 12, color);
  
  // Mercury (filled block at bottom)
  tft.fillRect(x + 1, y + 13, 13, 2, color);
}

void drawWaterDropletIcon(int x, int y, uint16_t color) {
  // Frame
  tft.drawRect(x, y, 15, 15, color);
  
  // Droplet outline (diamond shape)
  // Left diagonal
  tft.drawLine(x + 7, y + 2, x + 4, y + 7, color);
  // Right diagonal
  tft.drawLine(x + 8, y + 2, x + 11, y + 7, color);
  // Bottom left
  tft.drawLine(x + 4, y + 7, x + 7, y + 13, color);
  // Bottom right
  tft.drawLine(x + 11, y + 7, x + 7, y + 13, color);
  
  // Fill droplet
  tft.fillRect(x + 5, y + 5, 5, 7, color);
}

void drawHeatIcon(int x, int y, uint16_t color) {
  // Frame
  tft.drawRect(x, y, 15, 15, color);
  
  // Center point
  tft.fillRect(x + 6, y + 6, 3, 3, color);
  
  // Cross (vertical + horizontal)
  tft.drawLine(x + 7, y + 2, x + 7, y + 12, color);   // Vertical
  tft.drawLine(x + 2, y + 7, x + 12, y + 7, color);   // Horizontal
  
  // Diagonal points (X shape)
  tft.drawLine(x + 4, y + 4, x + 10, y + 10, color);  // \
  tft.drawLine(x + 10, y + 4, x + 4, y + 10, color);  // /
}

void drawRegionBorder(int region_y_start, int region_y_end, uint16_t color) {
  // Left border line (accent)
  tft.drawLine(2, region_y_start, 2, region_y_end, color);
  
  // Bottom divider line (separates from next region)
  tft.drawLine(0, region_y_end - 1, 159, region_y_end - 1, COLOR_GRID);
}

void drawCornerAccents() {
  // Top-left corner
  tft.drawLine(0, 0, 8, 0, COLOR_ACCENT_MAG);     // Horizontal
  tft.drawLine(0, 0, 0, 8, COLOR_ACCENT_MAG);     // Vertical
  
  // Top-right corner
  tft.drawLine(159, 0, 151, 0, COLOR_ACCENT_MAG);
  tft.drawLine(159, 0, 159, 8, COLOR_ACCENT_MAG);
  
  // Bottom-left corner
  tft.drawLine(0, 127, 8, 127, COLOR_ACCENT_MAG);
  tft.drawLine(0, 127, 0, 119, COLOR_ACCENT_MAG);
  
  // Bottom-right corner
  tft.drawLine(159, 127, 151, 127, COLOR_ACCENT_MAG);
  tft.drawLine(159, 127, 159, 119, COLOR_ACCENT_MAG);
}
```

---

## Step 4: Update Display Functions

Replace the temperature, humidity, and feels-like display functions:

```cpp
void updateTemperatureDisplay(float temp) {
  uint16_t color = getTempColor(temp);
  
  // Clear temperature region (rows 0-42)
  tft.fillRect(0, 0, 160, 42, COLOR_BLACK);
  
  // Redraw borders and icon
  drawThermometerIcon(10, 8, color);
  drawRegionBorder(0, 0, 42, color);
  
  // Draw large temperature value (text size 3)
  tft.setCursor(35, 5);
  tft.setTextColor(color);
  tft.setTextSize(3);
  tft.printf("%.1f", temp);
  
  // Draw degree symbol (small text size 2)
  tft.setCursor(125, 8);
  tft.setTextColor(color);
  tft.setTextSize(2);
  tft.print("C");
}

void updateHumidityDisplay(float humidity) {
  // Clear humidity region (rows 42-84)
  tft.fillRect(0, 42, 160, 42, COLOR_BLACK);
  
  // Redraw borders and icon
  drawWaterDropletIcon(10, 50, COLOR_HUMIDITY);
  drawRegionBorder(42, 42, 84, COLOR_HUMIDITY);
  
  // Draw large humidity value (text size 3)
  tft.setCursor(35, 47);
  tft.setTextColor(COLOR_HUMIDITY);
  tft.setTextSize(3);
  tft.printf("%.0f", humidity);
  
  // Draw percent symbol (text size 2)
  tft.setCursor(130, 50);
  tft.setTextColor(COLOR_HUMIDITY);
  tft.setTextSize(2);
  tft.print("%");
}

void updateFeelsLikeDisplay(float feels_like) {
  uint16_t color = getTempColor(feels_like);
  
  // Clear feels-like region (rows 84-128)
  tft.fillRect(0, 84, 160, 44, COLOR_BLACK);
  
  // Redraw borders and icon
  drawHeatIcon(10, 92, color);
  drawRegionBorder(84, 84, 128, color);
  
  // Draw large feels-like value (text size 3)
  tft.setCursor(35, 89);
  tft.setTextColor(color);
  tft.setTextSize(3);
  tft.printf("%.1f", feels_like);
  
  // Draw degree symbol (text size 2)
  tft.setCursor(125, 92);
  tft.setTextColor(color);
  tft.setTextSize(2);
  tft.print("C");
}
```

---

## Step 5: Fix Function Signature

Update `drawRegionBorder()` call signature to match. In `drawInitialUI()`, adjust:

```cpp
// BEFORE:
drawRegionBorder(0, 0, 42, COLOR_COLD);

// AFTER (correct parameters):
drawRegionBorder(0, 42, COLOR_COLD);  // y_start, y_end, color
```

Then update the function:

```cpp
void drawRegionBorder(int region_y_start, int region_y_end, uint16_t color) {
  // Left border line (accent)
  tft.drawLine(2, region_y_start, 2, region_y_end, color);
  
  // Bottom divider line
  tft.drawLine(0, region_y_end - 1, 159, region_y_end - 1, COLOR_GRID);
}
```

---

## Step 6: Optional - Add Grid Lines (Cyberpunk Effect)

Add this to `drawInitialUI()` for subtle CRT/HUD effect:

```cpp
void drawSubtleGrid() {
  // Optional: Vertical grid lines every 40 pixels
  for (int x = 40; x < 160; x += 40) {
    // Draw with very low opacity (use dimmer version of grid color)
    for (int y = 0; y < 128; y += 2) {
      tft.drawPixel(x, y, COLOR_GRID);  // Every other row
    }
  }
}
```

Call from `drawInitialUI()`:
```cpp
// Optional: Add subtle grid
// drawSubtleGrid();  // Uncomment for CRT effect
```

---

## Implementation Checklist

### Phase 1: Colors
- [ ] Add color definitions at top of file
- [ ] Verify RGB565 values render correctly

### Phase 2: Icon Functions
- [ ] Implement `drawThermometerIcon()`
- [ ] Implement `drawWaterDropletIcon()`
- [ ] Implement `drawHeatIcon()`
- [ ] Implement `drawRegionBorder()`
- [ ] Implement `drawCornerAccents()`

### Phase 3: Display Functions
- [ ] Replace `updateTemperatureDisplay()`
- [ ] Replace `updateHumidityDisplay()`
- [ ] Replace `updateFeelsLikeDisplay()`

### Phase 4: Main Layout
- [ ] Replace `drawInitialUI()`
- [ ] Test with mock values (0, 25, 50)
- [ ] Verify spacing and alignment
- [ ] Check colors render correctly

### Phase 5: Polish (Optional)
- [ ] Add `drawSubtleGrid()` for CRT effect
- [ ] Adjust icon sizes/positions if needed
- [ ] Tweak corner accents (remove if cluttered)

---

## Testing Sequence

```cpp
// Test 1: Initial render
// Upload code, check:
// ✓ Black background
// ✓ 3 icons visible (thermometer, droplet, heat)
// ✓ Blue/cyan borders visible
// ✓ Magenta corner accents visible
// ✓ Placeholder values "0.0" displayed

// Test 2: Temperature changes
// Modify DHT values in loop:
float temp = 15.0;  // Should show BLUE
float temp = 20.0;  // Should show GREEN
float temp = 28.0;  // Should show ORANGE
float temp = 32.0;  // Should show RED

// Test 3: Humidity static
// Check always CYAN, % symbol displays

// Test 4: Feels-like changes
// Should follow temperature color (< 26.7 = same as temp, > 26.7 = heat index)

// Test 5: Serial Monitor
// Verify T/H/F updates print correctly
```

---

## Performance Notes

**Draw Calls per Update:**
- Temperature: ~8 calls (fillRect, drawThermometerIcon, drawRegionBorder, setCursor, print)
- Humidity: ~8 calls
- Feels-Like: ~8 calls
- Total per cycle: ~24 draw calls (very efficient)

**Screen Redraws:**
- Full screen: Initialization only (~100 calls)
- Partial updates: ~8 calls per region per change
- Recommended minimum: Update every 500ms to avoid flicker

---

## Customization Options

### Reduce Clutter
```cpp
// Comment out corner accents
// drawCornerAccents();

// Or comment out grid
// drawSubtleGrid();
```

### Increase Icon Detail
```cpp
// Add more lines/shapes to icons
// Example: Thicker thermometer bulb
tft.fillRect(x + 1, y + 1, 13, 4, color);  // Instead of 3
```

### Change Color Scheme
```cpp
// Use magenta as accent instead of cyan
#define COLOR_HUMIDITY COLOR_ACCENT_MAG

// Or full neon pink theme
#define COLOR_COLD COLOR_ACCENT_MAG
```

---

## Debugging

**If icons don't render:**
- Check x,y coordinates match region positions
- Verify colors are defined before use
- Check `drawLine()` and `fillRect()` calls have correct parameters

**If text overlaps:**
- Increase x cursor position (shift right)
- Adjust text size from 3 to 2 if too large
- Use `setTextWrap(false)` to prevent wrapping

**If colors look wrong:**
- Verify RGB565 format: `0xRRRRGGGGBBBB` (5-6-5 bits)
- Test with pure colors: `0xFFFF` (white), `0x0000` (black)
- Check display rotation (`tft.setRotation(1)` for landscape)

---

## Final Notes

This design balances:
- ✅ **Minimal code** (simple shapes only)
- ✅ **Maximum visual impact** (neon colors, geometric icons)
- ✅ **High readability** (large text, high contrast)
- ✅ **Efficient updates** (partial screen redraws)
- ✅ **Scalable** (easy to customize colors/sizes)

The cyberpunk aesthetic emerges from:
- Pure black background (maximum contrast)
- Neon accent colors (cyan, green, orange, red)
- Geometric icons (no raster images needed)
- Thin accent lines (left border + corner marks)
- Tech-minimal layout (no gradients, no shadows)

Perfect for an embedded display showing real-time sensor data! 🎯
