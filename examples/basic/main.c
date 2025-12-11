#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "esp_err.h"
#include "quicklink.h"

static const char *TAG = "quicklink_example";

/**
 * @brief QuickLink event handler callback
 */
static void on_quicklink_event(quicklink_event_t event, void *data, void *user_ctx)
{
    switch (event) {
    case QUICKLINK_EVENT_STARTED:
        ESP_LOGI(TAG, "QuickLink service started");
        ESP_LOGI(TAG, "Device is now discoverable via BLE");
        break;

    case QUICKLINK_EVENT_STOPPED:
        ESP_LOGI(TAG, "QuickLink service stopped");
        break;

    case QUICKLINK_EVENT_BLE_STOPPED:
        ESP_LOGI(TAG, "BLE stopped (WiFi continues)");
        break;

    case QUICKLINK_EVENT_BLE_CONNECTED:
        ESP_LOGI(TAG, "BLE client connected");
        break;

    case QUICKLINK_EVENT_BLE_DISCONNECTED:
        ESP_LOGI(TAG, "BLE client disconnected");
        break;

    case QUICKLINK_EVENT_CRED_RECEIVED:
        ESP_LOGI(TAG, "WiFi credentials received from BLE client");
        break;

    case QUICKLINK_EVENT_WIFI_CONNECTING:
        ESP_LOGI(TAG, "Attempting to connect to WiFi...");
        break;

    case QUICKLINK_EVENT_WIFI_CONNECTED:
        ESP_LOGI(TAG, "Successfully connected to WiFi!");
        ESP_LOGI(TAG, "Provisioning complete - you can disconnect BLE client");
        break;

    case QUICKLINK_EVENT_WIFI_FAILED:
        ESP_LOGI(TAG, "WiFi connection failed - waiting for new credentials");
        break;

    default:
        ESP_LOGW(TAG, "Unknown QuickLink event: %d", event);
        break;
    }
}

/**
 * @brief Optional device information structure
 */
static const quicklink_device_info_t device_info = {
    .manufacturer = "Espressif Systems",
    .model_number = "ESP32-QuickLink",
    .serial_number = "QLE-001",
    .hardware_rev = "1.0",
    .firmware_rev = "1.0.0",
};

/**
 * @brief Application main entry point
 */
void app_main(void)
{
    ESP_LOGI(TAG, "Starting QuickLink example");
    ESP_LOGI(TAG, "Build timestamp: %s %s", __DATE__, __TIME__);

    // Initialize NVS Flash (required for WiFi credentials storage)
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_LOGW(TAG, "NVS needs format, erasing...");
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    // Configure QuickLink
    quicklink_config_t config = {
        .device_name = "ESP32-QuickLink",
        .enable_wifi_provisioning = true,  // Enable WiFi provisioning
        .ble_auto_stop = false,             // Don't auto-stop BLE after WiFi
        .event_cb = on_quicklink_event,
        .user_ctx = NULL,
        .device_info = &device_info,        // Optional device info
    };

    // Start QuickLink service
    ESP_LOGI(TAG, "Starting QuickLink with configuration:");
    ESP_LOGI(TAG, "  Device name: %s", config.device_name);
    ESP_LOGI(TAG, "  WiFi provisioning: %s", config.enable_wifi_provisioning ? "enabled" : "disabled");

    quicklink_status_t status = quicklink_start(&config);
    if (status != QUICKLINK_STATUS_OK) {
        ESP_LOGE(TAG, "Failed to start QuickLink: %d", status);
        return;
    }

    ESP_LOGI(TAG, "QuickLink initialized successfully");
    ESP_LOGI(TAG, "Open the provisioning app on your phone and scan for '%s'", config.device_name);

    // Main application loop
    while (1) {
        vTaskDelay(pdMS_TO_TICKS(5000));

        // Optional: Check status periodically
        bool is_running = quicklink_is_running();
        bool is_wifi_connected = quicklink_is_wifi_connected();

        ESP_LOGI(TAG, "Status - Running: %s, WiFi: %s",
                 is_running ? "yes" : "no",
                 is_wifi_connected ? "connected" : "disconnected");

        // Optional: Set battery level (if applicable to your hardware)
        // quicklink_set_battery_level(80);

        // Optional: Send a log message via BLE
        // quicklink_send_log("Device is running normally");
    }
}
