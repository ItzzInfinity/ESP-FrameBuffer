# Task 4.1: Quick Debug Start

## Current Status

Your output shows:
```
✓ SD card ready for logging     (but this is WRONG)
✗ Failed to open test file      (card not responding)
✗ Failed to create CSV file
```

**Problem**: CS pin is incorrect. SD.begin() succeeds but file operations fail.

## 3-Minute Fix

**The code now tests 4 CS pins automatically:**
```
GPIO 15 (most common for integrated adapters)
GPIO 3
GPIO 4
GPIO 8
```

**Step 1**: Upload the updated code
**Step 2**: Open Serial Monitor (115200 baud)
**Step 3**: Look for `✓✓✓ SD CARD WORKING! ✓✓✓`

If you see that → **Done!** Note the GPIO number, proceed to Task 5.

## If Still Failing

**Step 1: Check your display module documentation**

Look for:
- Pinout diagram
- Schematic
- SD card CS pin label (should say something like "CS", "D4", "GPIO_X")

**Step 2: Edit lines 35-38 with new CS pins**

```cpp
// Lines 35-38 in task4_1_SDCardDebug.ino
#define SD_CS_PRIMARY     15   // Change these
#define SD_CS_SECONDARY   3    // to match your
#define SD_CS_TERTIARY    4    // module's pins
#define SD_CS_FALLBACK    8
```

Try pins from your module documentation instead.

**Step 3: Re-upload and check output**

## Common CS Pins by Module

| Module | Likely CS Pin |
|--------|---------------|
| 1.8" TFT with SD back | GPIO 15, 3, 4 |
| Integrated ST7735 | GPIO 15, 4 |
| Display + SD combo | GPIO 10, 8 |
| Generic SPI SD | GPIO 5, 17 |

## Display Shows

```
T   | H   | Feels
    |     |
──────────────────
SD: ERROR
(Check serial monitor - it shows which GPIO worked)
```

Once SD works:
```
SD: OK (GPIO15)
31.3C | 51% | 34.2C
```

## Physical Checks

While code is testing:

1. **SD Card inserted fully?** (Should click when inserted)
2. **Card contacts clean?** (Gently wipe gold contacts)
3. **Different SD card?** (Try older smaller card like 4GB)
4. **Format card as FAT32?** (On computer, right-click → Format)

---

**Main goal**: Find the correct CS pin so SD.open() works.  
Serial output will tell you which one works!

