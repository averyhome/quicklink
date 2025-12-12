/**
 * @file example_full_features.c
 * @brief QuickLink example with Device Info, Battery Service and Custom Service
 */

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "quicklink.h"
#include "host/ble_uuid.h"
#include "host/ble_gatt.h"
#include "host/ble_hs.h"

static const char *TAG = "FULL_EXAMPLE";

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
    ESP_LOGI(TAG, "Custom characteristic accessed");
    
    if (ctxt->op == BLE_GATT_ACCESS_OP_READ_CHR) {
        int rc = os_mbuf_append(ctxt->om, custom_data, sizeof(custom_data));
        return rc == 0 ? 0 : BLE_ATT_ERR_INSUFFICIENT_RES;
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
            {0}  /* Terminator */
        }
    },
    {0}  /* Terminator */
};

/* Event handler */
static void event_handler(quicklink_event_t event, void *data, void *ctx)
{
    switch (event) {
        case QUICKLINK_EVENT_STARTED:
            ESP_LOGI(TAG, "✓ QuickLink started");
            ESP_LOGI(TAG, "  Services: Provisioning + DeviceInfo + Battery + Custom");
            quicklink_send_log("I FULL_EXAMPLE: QuickLink started - All services ready");
            break;

        case QUICKLINK_EVENT_BLE_CONNECTED:
            ESP_LOGI(TAG, "✓ BLE client connected");
            quicklink_send_log("I FULL_EXAMPLE: BLE client connected");
            break;

        case QUICKLINK_EVENT_CRED_RECEIVED:
            ESP_LOGI(TAG, "✓ WiFi credentials received");
            /* 不需要发送,因为 on_ble_data_received 已经发送了 */
            break;

        case QUICKLINK_EVENT_WIFI_CONNECTED:
            ESP_LOGI(TAG, "✓ WiFi connected!");
            /* 不需要发送,因为 on_wifi_result 已经发送了 */
            break;

        case QUICKLINK_EVENT_WIFI_FAILED:
            ESP_LOGE(TAG, "✗ WiFi connection failed");
            /* 不需要发送,因为 on_wifi_result 已经发送了 */
            break;

        case QUICKLINK_EVENT_STOPPED:
            ESP_LOGI(TAG, "✓ QuickLink stopped");
            quicklink_send_log("I FULL_EXAMPLE: QuickLink stopped");
            break;

        default:
            break;
    }
}

void app_main(void)
{
    ESP_LOGI(TAG, "===========================================");
    ESP_LOGI(TAG, "QuickLink Full Features Example");
    ESP_LOGI(TAG, "Version: %s", quicklink_get_version());
    ESP_LOGI(TAG, "===========================================");

    ESP_LOGI(TAG, "QuickLink started with:");
    ESP_LOGI(TAG, "  • Provisioning Service (WiFi配网)");
    ESP_LOGI(TAG, "  • Device Information Service (0x180A)");
    ESP_LOGI(TAG, "  • Battery Service (0x180F)");
    ESP_LOGI(TAG, "  • Custom Service (自定义服务)");
    ESP_LOGI(TAG, "");

    /* Define device information */
    static const quicklink_device_info_t device_info = {
        .manufacturer = "FireFrog",
        .model = "AirLink-Plus",
        .serial = "SN20231209001",
        .hw_version = "1.0",
        .fw_version = "1.0.0"  /* 使用常量字符串 */
    };

    /* Configure QuickLink with all features */
    quicklink_config_t config = {
        .device_name = "AirLink-Plus", 
        .event_cb = event_handler,
        .user_ctx = NULL,
        .ble_auto_stop = false,              /* Auto-stop BLE after WiFi connected */
        .enable_wifi_provisioning = true,   /* Enable WiFi provisioning (set false for BLE-only) */
        .device_info = &device_info,        /* Enable Device Info Service */
        .custom_services = custom_services  /* Add custom service */
    };

    /* Start QuickLink */
    quicklink_status_t ret = quicklink_start(&config);
    if (ret != QUICKLINK_OK) {
        ESP_LOGE(TAG, "Failed to start: %d", ret);
        return;
    }

    ESP_LOGI(TAG, "");
    ESP_LOGI(TAG, "💡 TIP: If you encounter bonding conflicts:");
    ESP_LOGI(TAG, "   1. Device will auto-reset on disconnect (reason 531/534)");
    ESP_LOGI(TAG, "   2. Or manually call: quicklink_reset_ble_identity()");
    ESP_LOGI(TAG, "   3. Then restart device to apply new identity");
    ESP_LOGI(TAG, "");

    /* Simulate battery level changes */
    return;
    uint8_t battery = 100;
    while (1) {
        vTaskDelay(pdMS_TO_TICKS(10000));

        /* Update battery level every 10 seconds */
        if (battery > 20) {
            battery -= 5;
        } else {
            battery = 100;  /* Reset for demo */
        }
        
        quicklink_set_battery_level(battery);
        ESP_LOGI(TAG, "Battery level updated: %d%%", battery);

        if (quicklink_is_wifi_connected()) {
            ESP_LOGI(TAG, "✓ WiFi is connected - app ready!");
        }
    }
}
