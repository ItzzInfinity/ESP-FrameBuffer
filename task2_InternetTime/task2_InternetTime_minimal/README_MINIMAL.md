# Task 2: Minimal Internet Clock

## What It Does

Simple, clean internet clock:
- ✅ Connects to WiFi once at startup
- ✅ Syncs time via NTP
- ✅ Uses internal clock after sync (no WiFi needed)
- ✅ Displays time without seconds (HH:MM format)
- ✅ Updates display every 1 minute (stable, not flickering)
- ✅ Uses GMT+5:30 (India Standard Time)
- ❌ No network scanning
- ❌ No multiple SSID logic
- ❌ No WiFi status updates

## Display

```
INTERNET CLOCK
──────────────────────────────
  14:32
  
  
  
  
  
  
  
Mon, Apr 17, 2026
──────────────────────────────
Updates every 1 min
```

## Code Size

- **Lines**: ~150
- **Time to understand**: 2 minutes
- **Complexity**: Minimal
- **Bloat removed**: ~350 lines compared to original

## Setup (30 seconds)

### 1. Edit WiFi Credentials (if different)
```cpp
Line 34-35:
#define SSID     "Itzz_Infinity"
#define PASSWORD "123456789"
```

### 2. Verify GMT Offset
```cpp
Line 36:
#define GMT_OFFSET 19800  // UTC+5:30 (India)
// Already set correctly!
```

### 3. Upload
```
Arduino IDE → Select Board → Upload
```

## What Happens at Startup

```
Task 2: Simple Internet Clock
Connecting to WiFi: Itzz_Infinity
....✓ WiFi Connected
IP: 192.168.1.100
Syncing time via NTP...
....✓ Time synced: Mon Apr 17 14:32:45 2026
WiFi disconnected (using internal clock)
```

Then display shows time.

## Display Update Behavior

- **Initial**: Shows time immediately after NTP sync
- **Every 60 seconds**: Refreshes display with latest time
- **Between updates**: Internal RTC keeps accurate time
- **Result**: Stable, no flickering, minimal power use

## Key Differences from Original

| Original | Minimal |
|----------|---------|
| 500+ lines | 150 lines |
| Multiple SSIDs | Single SSID |
| Constant WiFi checks | WiFi once at startup |
| Update every second | Update every 60 seconds |
| Status display | Simple time display |
| Complex logic | Straightforward code |

## Serial Output

```
Task 2: Simple Internet Clock
Connecting to WiFi: Itzz_Infinity
....✓ WiFi Connected
IP: 192.168.1.100
Syncing time via NTP...
....✓ Time synced: Mon Apr 17 14:32:45 2026
WiFi disconnected (using internal clock)
```

## Display Format

**Time Display:**
- Format: `HH:MM` (24-hour, no seconds)
- Size: Large (4x text size)
- Color: Yellow

**Date Display:**
- Format: `Day, Month Date, Year`
- Example: `Mon, Apr 17, 2026`
- Color: White
- Size: Normal

## Configuration Options

### Change WiFi
```cpp
#define SSID     "your_ssid"
#define PASSWORD "your_password"
```

### Change Timezone
```cpp
#define GMT_OFFSET 19800  // In seconds
// UTC+5:30 = 19800
// UTC+0 = 0
// UTC-5 = -18000
```

### Change Refresh Interval
```cpp
#define REFRESH_INTERVAL 60000  // milliseconds (60000 = 1 minute)
// Change to 30000 for 30 seconds
// Change to 300000 for 5 minutes
```

## Power Consumption

- **Startup**: ~120mA (WiFi connection + NTP sync) = ~3 seconds
- **Normal operation**: ~20-30mA (display + ESP32 idle)
- **Standby**: ~10-15mA (minimal)

**Battery life (500mAh):** ~20+ hours of continuous operation

## What Each Section Does

### `connectAndSyncTime()`
- Connects to WiFi using SSID/password
- Fetches time from NTP server
- Sets internal RTC
- Disconnects WiFi to save power
- Runs once at startup

### `displayTime()`
- Gets current time from internal clock
- Formats and displays on TFT
- Called every 60 seconds

### `loop()`
- Checks if 60 seconds elapsed
- Updates display if needed
- Otherwise does nothing

## Minimal Design Philosophy

✓ Only includes what was requested
✓ No error handling for impossible scenarios
✓ No "nice to have" features
✓ No unnecessary abstractions
✓ Clean, readable code
✓ Can be understood in 2 minutes

## Testing Checklist

After upload:
- [ ] Serial monitor shows WiFi connection
- [ ] Serial shows NTP sync successful
- [ ] Display shows correct time (HH:MM)
- [ ] Display shows correct date
- [ ] No display updates while watching (updates every 60 sec)
- [ ] After 1 minute, display refreshes

## Troubleshooting

| Problem | Solution |
|---------|----------|
| WiFi connection fails | Check SSID/password |
| Time is wrong | Check GMT_OFFSET value |
| Display blank | Check TFT wiring (same as Task 1) |
| Crashing | Check board selection (ESP32 vs ESP32-C3) |

---

**Total code**: ~150 lines
**Upload time**: ~5 seconds
**First time display**: Immediate after NTP sync
**Ready to use**: Copy and upload!
