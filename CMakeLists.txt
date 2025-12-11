idf_component_register(
    SRCS "dummy.c"
    INCLUDE_DIRS "include"
    REQUIRES
        nvs_flash
        bt
        mbedtls
        esp_wifi
        esp_event
        freertos
        esp_netif
        esp_hw_support
    PRIV_REQUIRES
        esp_driver_gpio
)

set(QUICKLINK_LIB_PATH "${CMAKE_CURRENT_SOURCE_DIR}/lib/libquicklink.a")

if(NOT EXISTS "${QUICKLINK_LIB_PATH}")
    message(FATAL_ERROR "QuickLink prebuilt library not found: ${QUICKLINK_LIB_PATH}")
endif()

# 注册预编译静态库，并声明它的依赖，保证链接顺序正确（消费方在前，依赖在后）
add_prebuilt_library(quicklinklib "${QUICKLINK_LIB_PATH}" REQUIRES
    nvs_flash
    bt
    mbedtls
    esp_wifi
    esp_event
    freertos
    esp_netif
    esp_hw_support
)

# 让本组件依赖预编译库
target_link_libraries(${COMPONENT_LIB} PUBLIC quicklinklib)