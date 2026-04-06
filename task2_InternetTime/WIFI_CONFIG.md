# WiFi Network Configuration Guide

## Overview
This file explains how to manage WiFi networks in Task 2 using the flexible network array system.

## Current Configuration

Your current WiFi network array in `task2_InternetTime.ino`:

```cpp
const WiFiNetwork WIFI_NETWORKS[] = {
  {"Itzz_Infinity", "123456789"},  // Primary network
};
```

This tells the system:
- Try to connect to "Itzz_Infinity" with password "123456789"
- If this network is unavailable, show error and retry

---

## How to Add More Networks

### Step 1: Identify Your Networks
List your WiFi networks and passwords:
1. **Home WiFi** - SSID: `Itzz_Infinity`, Password: `123456789`
2. **Mobile Hotspot** - SSID: `MyPhone`, Password: `hotspot_pass`
3. **Office WiFi** - SSID: `CompanyWiFi`, Password: `office_pass`

### Step 2: Add to Network Array

Open `task2_InternetTime.ino` and find:
```cpp
const WiFiNetwork WIFI_NETWORKS[] = {
  {"Itzz_Infinity", "123456789"},  // Primary network
  // Add additional networks below (uncomment and fill in your networks)
  // {"Network_Name_2", "Password_2"},
  // {"Network_Name_3", "Password_3"},
};
```

Replace with your networks:
```cpp
const WiFiNetwork WIFI_NETWORKS[] = {
  {"Itzz_Infinity", "123456789"},  // Home WiFi (primary)
  {"MyPhone", "hotspot_pass"},     // Mobile hotspot (fallback)
  {"CompanyWiFi", "office_pass"},  // Office Network (backup)
};
```

### Step 3: Save and Upload
- Save the file
- Upload to ESP32/ESP32C3
- The system will automatically try networks in this order

---

## Network Connection Priority

**The system tries networks in order:**
1. First, it scans for all available WiFi networks
2. Then matches them against your WIFI_NETWORKS array
3. Connects to the first match found (in array order)

**Example with 3 networks:**

```cpp
const WiFiNetwork WIFI_NETWORKS[] = {
  {"HomeWiFi", "home_pass"},      // [1] Try this FIRST
  {"OfficeWiFi", "office_pass"},  // [2] Try this SECOND (if HomeWiFi not available)
  {"MobileHotspot", "mobile_pass"},// [3] Try this LAST (if others not available)
};
```

**Scenario:** If HomeWiFi is down but OfficeWiFi and MobileHotspot are available:
- System automatically connects to OfficeWiFi (not MobileHotspot)
- This is useful for work-from-home or multi-location setups

---

## Common Network Configurations

### Single Network (Current - Minimal)
```cpp
const WiFiNetwork WIFI_NETWORKS[] = {
  {"Itzz_Infinity", "123456789"},
};
const int NUM_NETWORKS = 1;
```
**Use when:** Only have one WiFi network
**Pro:** Simple, minimal code
**Con:** Fails if that network is unavailable

### Dual Network (Recommended)
```cpp
const WiFiNetwork WIFI_NETWORKS[] = {
  {"Itzz_Infinity", "123456789"},     // Primary (home)
  {"MobileHotspot", "hotspot_pass"},  // Fallback (mobile)
};
const int NUM_NETWORKS = 2;
```
**Use when:** Have home + mobile hotspot backup
**Pro:** Automatic fallback, good reliability
**Con:** Two networks to maintain

### Triple Network (Business/Multiple Locations)
```cpp
const WiFiNetwork WIFI_NETWORKS[] = {
  {"HomeWiFi", "home_pass"},          // Primary
  {"OfficeWiFi", "office_pass"},      // Secondary
  {"GuestNetwork", "guest_pass"},     // Backup
};
const int NUM_NETWORKS = 3;
```
**Use when:** Work from different locations
**Pro:** Maximum flexibility and coverage
**Con:** More maintenance, passwords to remember

### Four+ Networks (Advanced Multi-Location)
```cpp
const WiFiNetwork WIFI_NETWORKS[] = {
  {"HomeWiFi", "home_pass"},
  {"OfficeWiFi", "office_pass"},
  {"CafeWiFi", "cafe_pass"},
  {"MobileHotspot", "mobile_pass"},
  {"SecondaryOffice", "alt_pass"},
};
const int NUM_NETWORKS = 5;
```
**Use when:** Frequently traveling with device
**Pro:** Works everywhere you go automatically
**Con:** High complexity, security risk (many passwords stored)

---

## Password Requirements

WiFi passwords must follow these rules:

| Requirement | Details |
|-------------|---------|
| **Length** | 8-63 characters |
| **Format** | ASCII text only (no Unicode) |
| **Special Characters** | Allowed: `!@#$%^&*()_+-=[]{};\:'"\|,.<>?` |
| **Case Sensitive** | YES - "Password123" ≠ "password123" |
| **Spaces** | Allowed anywhere in password |

**Examples:**

✅ **Valid Passwords:**
- `123456789` (your current password)
- `MyWiFi@Home2024` (alphanumeric with special char)
- `correct horse battery staple` (spaces allowed)
- `P@ssw0rd!` (mixed with special characters)

❌ **Invalid Passwords:**
- `12345` (too short - minimum 8 characters)
- `123456789012345678901234567890123456789012345678901234567890123456` (too long - maximum 63 characters)

---

## Network Management Examples

### Adding a Network (Quick Reference)
Before:
```cpp
const WiFiNetwork WIFI_NETWORKS[] = {
  {"Itzz_Infinity", "123456789"},
};
```

After (adding backup):
```cpp
const WiFiNetwork WIFI_NETWORKS[] = {
  {"Itzz_Infinity", "123456789"},
  {"BackupNetwork", "backup_pass"},
};
```

### Removing a Network
Simply delete the line (including the comma):
```cpp
// BEFORE
const WiFiNetwork WIFI_NETWORKS[] = {
  {"Itzz_Infinity", "123456789"},
  {"OldNetwork", "old_pass"},       // ← Remove this
  {"BackupNetwork", "backup_pass"},
};

// AFTER
const WiFiNetwork WIFI_NETWORKS[] = {
  {"Itzz_Infinity", "123456789"},
  {"BackupNetwork", "backup_pass"},
};
```

### Reordering Networks
Change the order to change connection priority:
```cpp
// If HomeWiFi fails more often, put mobile first:
const WiFiNetwork WIFI_NETWORKS[] = {
  {"MobileHotspot", "hotspot_pass"},  // Try mobile first (most reliable)
  {"HomeWiFi", "home_pass"},          // Try home second
};
```

### Temporarily Disable a Network
Comment it out without deleting:
```cpp
const WiFiNetwork WIFI_NETWORKS[] = {
  {"Itzz_Infinity", "123456789"},
  // {"OldNetwork", "old_pass"},     // Disabled for now
  {"BackupNetwork", "backup_pass"},
};
```

---

## Automatic Behavior

The system automatically:

✅ **Scans for networks** before trying to connect
- Only attempts connection if network is detected
- Avoids wasting time trying unavailable networks

✅ **Falls back gracefully** if network fails
- Tries next network in array
- No complete failure if some networks are down

✅ **Reconnects automatically** if WiFi drops
- Every 10 seconds, checks WiFi status
- Reconnects immediately if disconnected

✅ **Shows status on display**
- Green "WiFi" = Connected
- Red "X NO WiFi" = Disconnected
- Network name, IP, and signal strength = Always visible

---

## Security Notes

### IMPORTANT: Password Storage
This code stores WiFi passwords in plain text. This is generally **acceptable** for:
- Personal home networks
- Testing environments
- Non-critical deployments

This is **NOT recommended** for:
- Critical infrastructure
- High-security applications
- Shared/public networks

### Best Practices
1. Use a separate WiFi network for IoT devices (guest network)
2. Change password regularly
3. Don't use same password for other services
4. For production, consider encrypted credential storage
5. Review compiled binary security if in high-risk environment

---

## Troubleshooting Network Issues

### Network Not Found
**Symptom:** Serial output shows "Network not available: Itzz_Infinity"

**Causes & Solutions:**
- ✓ WiFi router is off → Turn it on
- ✓ Device is out of range → Move closer
- ✓ SSID spelling is wrong → Check exact spelling (case-sensitive)
- ✓ 5GHz network selected → Use 2.4GHz band
- ✓ WiFi is in hidden broadcast mode → Add SSID manually

**To Fix:**
```cpp
// Instead of relying on scan, can also try hardcoded:
WiFi.begin("Itzz_Infinity", "123456789");
```

### Connection Fails with Correct Password
**Symptom:** Tries for 10 seconds then moves to next network

**Causes & Solutions:**
- ✓ Interference/poor signal → Move device closer or check for interference
- ✓ Too many devices on router → Disconnect other devices
- ✓ Router security settings → Set to WPA2/WPA3, disable MAC filtering
- ✓ Character encoding issue → Re-type password (not copy-paste)

**To Debug:**
Check Serial output for detailed error messages. Each connection attempt prints a dot `.` - missing dots indicate connection drops.

### Keeps Connecting and Disconnecting
**Symptom:** WiFi connects briefly, then disconnects, repeats cyclically

**Causes & Solutions:**
- ✓ Poor signal → Move closer to router
- ✓ Interference → Move away from microwave/cordless phone
  
**To Fix:**
- Add delay in `connectToWiFi()` before WiFi.begin()
- Reduce `WIFI_CHECK_INTERVAL` to detect faster

---

## Configuration Checklists

### Before First Upload (Single Network Setup)
- [ ] Found your WiFi SSID (network name)
- [ ] Located WiFi password
- [ ] Replaced "Itzz_Infinity" with your SSID
- [ ] Replaced "123456789" with your password
- [ ] Password is 8-63 characters
- [ ] Password doesn't contain special Unicode characters
- [ ] Saved the file

### Before Adding Second Network
- [ ] Know second network's SSID
- [ ] Know second network's password
- [ ] Added new line to array with exact syntax
- [ ] Included comma after first network
- [ ] Verified password length (8-63 chars)
- [ ] Saved and uploaded

### For Frequent Travelers
- [ ] Listed all networks you'll visit
- [ ] Ordered by reliability (most reliable first)
- [ ] Tested connectivity at each location
- [ ] Have contact info for network passwords
- [ ] Consider using mobile hotspot as last resort

---

## Advanced: Custom Network Scanning

Want to see what networks are available? Add this to analyze before configuring:

```cpp
void scanNetworks() {
  int n = WiFi.scanNetworks();
  Serial.println("Available Networks:");
  for (int i = 0; i < n; i++) {
    Serial.print(i + 1);
    Serial.print(". SSID: ");
    Serial.print(WiFi.SSID(i));
    Serial.print(" (");
    Serial.print(WiFi.RSSI(i));
    Serial.println(" dBm)");
  }
}
```

Add to `setup()` before `connectToWiFi()` to log available networks.

---

## Support

For network-related issues:
1. Check Serial Monitor output (115200 baud)
2. Verify SSID and password spelling
3. Test WiFi with another device (phone, laptop)
4. Restart WiFi router
5. Check if 2.4GHz band is available on router
