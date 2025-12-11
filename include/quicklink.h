/**
 * @file quicklink.h
 * @brief QuickLink - Simple BLE WiFi Provisioning Component
 * 
 * A lightweight component for WiFi provisioning via BLE (NimBLE)
 * Supports all ESP32 targets with WiFi capability
 */

#ifndef QUICKLINK_H
#define QUICKLINK_H

#include <stdint.h>
#include <stdbool.h>
#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief QuickLink status codes
 */
typedef enum {
    QUICKLINK_OK = 0,                  /**< Operation successful */
    QUICKLINK_ERR_INVALID_PARAM,       /**< Invalid parameter */
    QUICKLINK_ERR_NO_MEM,              /**< Out of memory */
    QUICKLINK_ERR_BLE_FAILED,          /**< BLE operation failed */
    QUICKLINK_ERR_WIFI_FAILED,         /**< WiFi operation failed */
    QUICKLINK_ERR_ALREADY_RUNNING,     /**< Service already running */
    QUICKLINK_ERR_NOT_RUNNING,         /**< Service not running */
} quicklink_status_t;

/**
 * @brief QuickLink events
 */
typedef enum {
    QUICKLINK_EVENT_STARTED,         /**< Service started */
    QUICKLINK_EVENT_STOPPED,         /**< Service stopped (BLE + WiFi) */
    QUICKLINK_EVENT_BLE_STOPPED,     /**< BLE service stopped (WiFi continues) */
    QUICKLINK_EVENT_BLE_CONNECTED,   /**< BLE client connected */
    QUICKLINK_EVENT_BLE_DISCONNECTED,/**< BLE client disconnected */
    QUICKLINK_EVENT_CRED_RECEIVED,   /**< Credentials received */
    QUICKLINK_EVENT_WIFI_CONNECTING, /**< WiFi connecting */
    QUICKLINK_EVENT_WIFI_CONNECTED,  /**< WiFi connected */
    QUICKLINK_EVENT_WIFI_FAILED,     /**< WiFi failed */
} quicklink_event_t;

/**
 * @brief WiFi credentials
 */
typedef struct {
    char ssid[33];      /**< SSID (max 32 chars) */
    char password[65];  /**< Password (max 64 chars) */
} quicklink_credentials_t;

/**
 * @brief Device information for BLE Device Information Service (0x180A)
 */
typedef struct quicklink_device_info_s {
    const char *manufacturer;   /**< Manufacturer name (optional) */
    const char *model;          /**< Model number (optional) */
    const char *serial;         /**< Serial number (optional) */
    const char *hw_version;     /**< Hardware version (optional) */
    const char *fw_version;     /**< Firmware version (optional) */
} quicklink_device_info_t;

/**
 * @brief Event callback function
 * 
 * @param event Event type
 * @param data Event data (can be NULL)
 * @param user_ctx User context
 */
typedef void (*quicklink_event_cb_t)(quicklink_event_t event, void *data, void *user_ctx);

/**
 * @brief QuickLink configuration
 */
typedef struct {
    const char *device_name;                /**< BLE device name */
    quicklink_event_cb_t event_cb;          /**< Event callback */
    void *user_ctx;                         /**< User context */
    bool ble_auto_stop;                     /**< Auto-stop BLE after WiFi connected (default: false) */
    bool enable_wifi_provisioning;          /**< Enable WiFi provisioning (default: true, set false for BLE-only mode) */
    const quicklink_device_info_t *device_info;  /**< Device info (optional) */
    const void *custom_services;            /**< Custom GATT services (optional, requires NimBLE knowledge) */
} quicklink_config_t;

/**
 * @brief Start QuickLink service
 * 
 * @param config Configuration
 * @return Status code
 */
quicklink_status_t quicklink_start(const quicklink_config_t *config);

/**
 * @brief Stop QuickLink service (stops both BLE and WiFi)
 * 
 * @return Status code
 */
quicklink_status_t quicklink_stop(void);

/**
 * @brief Stop only BLE service, keep WiFi connected
 * 
 * Useful after WiFi provisioning is complete to save power
 * while maintaining WiFi connection.
 * 
 * @return Status code
 */
quicklink_status_t quicklink_stop_ble(void);

/**
 * @brief Check if service is running
 * 
 * @return true if running
 */
bool quicklink_is_running(void);

/**
 * @brief Check WiFi connection status
 * 
 * @return true if connected
 */
bool quicklink_is_wifi_connected(void);

/**
 * @brief Get component version
 * 
 * @return Version string
 */
const char* quicklink_get_version(void);

/**
 * @brief Set battery level for BLE Battery Service (0x180F)
 * 
 * Updates battery level and notifies connected BLE clients.
 * 
 * @param level Battery level (0-100%)
 */
void quicklink_set_battery_level(uint8_t level);

/**
 * @brief Send a log message to connected BLE client
 * 
 * This function sends a log message through BLE to the connected iOS/Android client.
 * The message will only be sent if:
 * - A BLE client is connected
 * - The log characteristic is subscribed (notifications enabled)
 * - No recursive logging is in progress
 * 
 * @param message Log message string (max 255 bytes)
 * 
 * @note This is a manual logging API. Call it explicitly for important application logs.
 * @note The message should be formatted like: "I TAG: message" or "E TAG: message"
 */
void quicklink_send_log(const char *message);

/**
 * @brief Reset BLE identity (clear bonding and regenerate random address)
 * 
 * Use this to make the device appear as a new device to BLE clients.
 * This clears all bonding information and generates a new random MAC address.
 * Call this before quicklink_start() or after quicklink_stop().
 * 
 * @return QUICKLINK_OK on success, error code otherwise
 */
quicklink_status_t quicklink_reset_ble_identity(void);

/// Added function to disconnect WiFi
esp_err_t quicklink_disconnect_wifi(void);

esp_err_t quicklink_reconnect_wifi(void);

#ifdef __cplusplus
}
#endif

#endif /* QUICKLINK_H */
