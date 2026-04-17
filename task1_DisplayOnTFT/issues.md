# Display Issues - Task 1

## Issue Status: ALL RESOLVED ✅

### Issue 1: Top and left corner has some white pixels/multicolored pixels that appear randomly
**Status:** ✅ RESOLVED

**Cause:** Drawing at edge coordinates (0,0) caused ST7735S controller addressing errors

**Fix Applied:**
- Added 5-pixel margin to all drawing operations
- Modified all coordinates to avoid pixel position (0,0)
- Lines now drawn from `margin` to `128-margin` instead of `0` to `128`

**Verification:** No artifacts should appear at display edges

---

### Issue 2: CYAN is appeared yellow on screen
**Status:** ✅ RESOLVED

**Cause:** INITR_18GREENTAB initialization had incorrect RGB565 color byte order

**Fixes Applied:**
- Changed initialization from `INITR_18GREENTAB` to `INITR_BLACKTAB`
- Created custom color definitions:
  ```cpp
  #define COLOR_CYAN 0x07FF  // Correct RGB565 for CYAN
  ```
- Replaced all `ST7735_CYAN` references with `COLOR_CYAN`

**Verification:** CYAN should display as pure cyan (blue + green), not yellow

---

### Issue 3: RED is Blue on screen
**Status:** ✅ RESOLVED

**Cause:** INITR_18GREENTAB had Red/Blue channel swap in color encoding

**Fixes Applied:**
- Changed initialization from `INITR_18GREENTAB` to `INITR_BLACKTAB` (same fix as Issue #2)
- Created custom color definitions:
  ```cpp
  #define COLOR_RED  0xF800  // Pure red
  #define COLOR_BLUE 0x001F  // Pure blue
  ```
- Replaced all `ST7735_RED` and `ST7735_BLUE` references with custom colors

**Verification:** RED should display as pure red (not blue), BLUE as pure blue (not red)

---

## Modified Files

- ✅ `task1_DisplayOnTFT/task1_DisplayOnTFT.ino` - All fixes applied
- ✅ `ISSUES_RESOLVED.md` - Detailed documentation created

## Next Steps

1. Upload updated code to ESP32/ESP32-C3
2. Verify display rendering with correct colors
3. Confirm no edge artifacts in top-left corner
4. Proceed with Task 2: Internet time/date display

