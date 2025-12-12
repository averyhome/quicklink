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

## Air Link iOS App - Quick WiFi Provisioning via BLE

<p align="center">
  <img src="images/6-5%20Template%2054.png" width="250" alt="App Screenshot 1"/>
  <img src="images/6-5%20Template%2055.png" width="250" alt="App Screenshot 2"/>
  <img src="images/6-5%20Template%2056.png" width="250" alt="App Screenshot 3"/>
</p>

Use the Air Link iOS app to easily provision WiFi credentials to your ESP32 device via BLE. Simply scan for your device, connect, and configure WiFi settings securely.

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
    path: "components/quicklink"
    version: "^1.0.2"
```

Then rebuild your project:

```bash
idf.py build
```

## Quick Start

### 1. Basic Example

```c
#include "quicklink.h"

static void event_handler(quicklink_event_t event, void *data, void *ctx)
{
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
            ESP_LOGE(TAG, "WiFi connection failed");
            break;
        case QUICKLINK_EVENT_STOPPED:
            ESP_LOGI(TAG, "QuickLink stopped");
            break;
        default:
            break;
    }
}

void app_main(void) {
    quicklink_config_t config = {
        .device_name = "MyDevice",
        .enable_wifi_provisioning = true,
        .ble_auto_stop = false,
        .event_cb = event_handler,
        .user_ctx = NULL,
    };
    
    quicklink_status_t ret = quicklink_start(&config);
    if (ret != QUICKLINK_OK) {
        ESP_LOGE(TAG, "Failed to start QuickLink: %d", ret);
        return;
    }
}
```

### 2. Full-Featured Example

With Device Info, Battery Service, and Custom GATT Service:

```c
#include "quicklink.h"
#include "host/ble_uuid.h"
#include "host/ble_gatt.h"

/* Custom Service UUID: 12345678-1234-1234-1234-123456789abc */
static const ble_uuid128_t custom_svc_uuid =
    BLE_UUID128_INIT(0xbc, 0x9a, 0x78, 0x56, 0x34, 0x12, 0x34, 0x12,
                     0x34, 0x12, 0x34, 0x12, 0x78, 0x56, 0x34, 0x12);

/* Custom Characteristic UUID: 12345678-1234-1234-1234-123456789abd */
static const ble_uuid128_t custom_chr_uuid =
    BLE_UUID128_INIT(0xbd, 0x9a, 0x78, 0x56, 0x34, 0x12, 0x34, 0x12,
                     0x34, 0x12, 0x34, 0x12, 0x78, 0x56, 0x34, 0x12);

static uint8_t custom_data[20] = "Hello QuickLink!";

/* Custom characteristic access callback */
static int custom_chr_access(uint16_t conn_handle, uint16_t attr_handle,
                             struct ble_gatt_access_ctxt *ctxt, void *arg)
{
    if (ctxt->op == BLE_GATT_ACCESS_OP_READ_CHR) {
        return os_mbuf_append(ctxt->om, custom_data, sizeof(custom_data)) == 0 
               ? 0 : BLE_ATT_ERR_INSUFFICIENT_RES;
    } else if (ctxt->op == BLE_GATT_ACCESS_OP_WRITE_CHR) {
        uint16_t len = OS_MBUF_PKTLEN(ctxt->om);
        if (len > sizeof(custom_data)) len = sizeof(custom_data);
        ble_hs_mbuf_to_flat(ctxt->om, custom_data, len, NULL);
        ESP_LOGI(TAG, "Custom data written: %s", custom_data);
        return 0;
    }
    return BLE_ATT_ERR_UNLIKELY;
}

/* Define custom GATT service */
static const struct ble_gatt_svc_def custom_services[] = {
    {
        .type = BLE_GATT_SVC_TYPE_PRIMARY,
        .uuid = &custom_svc_uuid.u,
        .characteristics = (struct ble_gatt_chr_def[]) {
            {
                .uuid = &custom_chr_uuid.u,
                .access_cb = custom_chr_access,
                .flags = BLE_GATT_CHR_F_READ | BLE_GATT_CHR_F_WRITE,
            },
            {0}
        }
    },
    {0}
};

void app_main(void)
{
    /* Define device information */
    static const quicklink_device_info_t device_info = {
        .manufacturer = "YourCompany",
        .model = "YourModel",
        .serial = "SN20240101001",
        .hw_version = "1.0",
        .fw_version = "1.0.0"
    };

    /* Configure QuickLink with all features */
    quicklink_config_t config = {
        .device_name = "MyDevice",
        .event_cb = event_handler,
        .user_ctx = NULL,
        .ble_auto_stop = false,
        .enable_wifi_provisioning = true,
        .device_info = &device_info,        /* Enable Device Info Service */
        .custom_services = custom_services  /* Add custom GATT services */
    };

    quicklink_status_t ret = quicklink_start(&config);
    if (ret != QUICKLINK_OK) {
        ESP_LOGE(TAG, "Failed to start QuickLink: %d", ret);
        return;
    }

    /* Update battery level periodically */
    uint8_t battery = 100;
    while (1) {
        vTaskDelay(pdMS_TO_TICKS(10000));
        battery = (battery > 20) ? (battery - 5) : 100;
        quicklink_set_battery_level(battery);
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
