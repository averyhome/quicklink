idf_component_register(
    INCLUDE_DIRS "include"
    REQUIRES nvs_flash bt mbedtls esp_wifi esp_event
)

# Link precompiled static library
target_link_libraries(${COMPONENT_LIB} PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/libquicklink.a
)

# Ensure library path exists
if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/lib/libquicklink.a")
    message(FATAL_ERROR "libquicklink.a not found. Please rebuild QuickLink component first.")
endif()
