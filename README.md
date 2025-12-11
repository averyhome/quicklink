# QuickLink - Simple BLE WiFi Provisioning Component

QuickLink is a lightweight, easy-to-use BLE WiFi provisioning component for ESP32 family microcontrollers. It enables simple and secure WiFi credential provisioning through Bluetooth Low Energy (BLE).

## Features

- **BLE-based WiFi Provisioning** - Configure WiFi credentials via BLE without needing pre-configured credentials
- **ECDH Encryption** - Secure credential transmission with ECDH key exchange and AES-GCM encryption
- **BLE-Only Mode** - Works on WiFi-less ESP32 variants (ESP32-C2, etc.)
- **Multi-device Support** - Handle multiple device connections simultaneously
- **Battery Service** - Built-in BLE Battery Service support
- **Real-time Logging** - Stream device logs via BLE to mobile clients
- **Device Information Service** - Standard BLE device info (manufacturer, model, firmware, etc.)
- **Minimal Dependencies** - Only requires NimBLE, mbedTLS, and NVS Flash

## Supported Targets

- ESP32
- ESP32-S2
- ESP32-S3
- ESP32-C2
- ESP32-C3
- ESP32-C6

## Installation

Add QuickLink to your project's `idf_component.yml`:

```yaml
dependencies:
  quicklink:
    git: "https://github.com/averyhome/quicklink.git"
    version: "^1.0.0"
```

Then rebuild your project:

```bash
idf.py build
```

## Quick Start

### 1. Initialize QuickLink

```c
#include "quicklink.h"

void app_main(void) {
    quicklink_config_t config = {
        .device_name = "MyDevice",
        .enable_wifi_provisioning = true,
        .ble_auto_stop = false,
        .event_cb = on_quicklink_event,
        .user_ctx = NULL,
    };
    
    quicklink_start(&config);
}
```

### 2. Handle Events

```c
void on_quicklink_event(quicklink_event_t event, void *data, void *user_ctx) {
    switch (event) {
    case QUICKLINK_EVENT_STARTED:
        ESP_LOGI(TAG, "QuickLink started");
        break;
    case QUICKLINK_EVENT_BLE_CONNECTED:
        ESP_LOGI(TAG, "BLE client connected");
        break;
    case QUICKLINK_EVENT_CRED_RECEIVED:
        ESP_LOGI(TAG, "WiFi credentials received");
        break;
    case QUICKLINK_EVENT_WIFI_CONNECTED:
        ESP_LOGI(TAG, "WiFi connected successfully");
        break;
    case QUICKLINK_EVENT_WIFI_FAILED:
        ESP_LOGI(TAG, "WiFi connection failed");
        break;
    default:
        break;
    }
}
```

### 3. Configuration Options

Enable/disable features via `menuconfig`:

```bash
idf.py menuconfig
# Navigate to Component Config → QuickLink
```

**QUICKLINK_ENABLE_WIFI** (default: enabled)
- Enable WiFi provisioning functionality
- Disable for BLE-only builds on WiFi-less chips

**QUICKLINK_STRIP_DEBUG** (default: enabled)
- Strip debug symbols to reduce binary size and protect code

**QUICKLINK_WIFI_MAX_RETRY** (default: 5)
- Maximum WiFi connection retry attempts

## API Reference

### Configuration

```c
typedef struct {
    const char *device_name;                    // BLE device name (required)
    quicklink_event_cb_t event_cb;              // Event callback
    void *user_ctx;                             // User context
    bool ble_auto_stop;                         // Auto-stop BLE after WiFi connected
    bool enable_wifi_provisioning;              // Enable WiFi provisioning
    const quicklink_device_info_t *device_info; // Optional device info
    const void *custom_services;                // Optional custom GATT services
} quicklink_config_t;
```

### Core Functions

```c
// Start QuickLink service
quicklink_status_t quicklink_start(const quicklink_config_t *config);

// Stop both BLE and WiFi
quicklink_status_t quicklink_stop(void);

// Stop only BLE (keep WiFi connected)
quicklink_status_t quicklink_stop_ble(void);

// Check if running
bool quicklink_is_running(void);

// Check WiFi connection status
bool quicklink_is_wifi_connected(void);

// Set battery level (0-100%)
void quicklink_set_battery_level(uint8_t level);

// Send log message via BLE
void quicklink_send_log(const char *message);

// Reset BLE identity
quicklink_status_t quicklink_reset_ble_identity(void);
```

## Events

```c
typedef enum {
    QUICKLINK_EVENT_STARTED,         // Service started
    QUICKLINK_EVENT_STOPPED,         // Service stopped
    QUICKLINK_EVENT_BLE_STOPPED,     // BLE stopped (WiFi continues)
    QUICKLINK_EVENT_BLE_CONNECTED,   // BLE client connected
    QUICKLINK_EVENT_BLE_DISCONNECTED,// BLE client disconnected
    QUICKLINK_EVENT_CRED_RECEIVED,   // WiFi credentials received
    QUICKLINK_EVENT_WIFI_CONNECTING, // WiFi connecting
    QUICKLINK_EVENT_WIFI_CONNECTED,  // WiFi connected
    QUICKLINK_EVENT_WIFI_FAILED,     // WiFi failed
} quicklink_event_t;
```

## BLE Services and Characteristics

### QuickLink Provisioning Service (6e400001-b5a3-f393-e0a9-e50e24dcca9e)

| UUID | Type | Direction | Purpose |
|------|------|-----------|---------|
| 6e400002 | WRITE | Client → Device | WiFi SSID |
| 6e400003 | WRITE | Client → Device | WiFi Password (plaintext, deprecated) |
| 6e400004 | READ/NOTIFY | Device → Client | Connection Status |
| 6e400005 | READ | Device → Client | Server Public Key (ECDH) |
| 6e400006 | WRITE | Client → Device | Client Public Key (ECDH) |
| 6e400007 | WRITE | Client → Device | Encrypted Password |
| 6e400008 | NOTIFY | Device → Client | Log Output |

### Standard Services

- **Device Information Service (0x180A)** - Manufacturer, model, serial, hardware/firmware version
- **Battery Service (0x180F)** - Battery level reporting

## Examples

See the `examples/` folder for complete example projects:

- **basic** - Simple WiFi provisioning example
- **advanced** - Full-featured example with logging and device info

## Troubleshooting

### "WiFi provisioning disabled by Kconfig" warning

WiFi support is disabled. Set `CONFIG_QUICKLINK_ENABLE_WIFI=y` in menuconfig for WiFi functionality, or use BLE-only mode by setting it to `n`.

### Device not appearing in BLE scan

Ensure:
1. BLE is enabled: `CONFIG_BT_ENABLED=y`
2. NimBLE is enabled: `CONFIG_BT_NIMBLE_ENABLED=y`
3. Device has sufficient free heap memory
4. Bluetooth permissions are granted on mobile device

### WiFi connection fails

1. Check SSID and password are correct
2. Verify WiFi network is 2.4GHz (most ESP32s don't support 5GHz)
3. Check WiFi signal strength at device location
4. Review logs: `idf.py monitor`

## License

Proprietary - See LICENSE file for details

## Support

For issues, questions, or feedback, please visit:
- GitHub Issues: https://github.com/averyhome/quicklink/issues
- Documentation: https://github.com/averyhome/quicklink/wiki

## Changelog

See [CHANGELOG.md](CHANGELOG.md) for version history and updates.
