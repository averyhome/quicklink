# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [1.0.0] - 2025-01-XX

### Added

- Initial public release of QuickLink component
- BLE-based WiFi provisioning with ECDH-AES-GCM encryption
- ECDH key exchange with public key verification
- HKDF-based key derivation for encryption
- BLE-only mode for WiFi-less ESP32 chips (ESP32-C2, etc.)
- Device Information Service (BLE DIS standard)
- Battery Service (BLE standard)
- Real-time device log streaming via BLE characteristic
- Multi-device connection support
- Configurable WiFi retry attempts
- Optional debug symbol stripping for size optimization
- Event-based callback system for provisioning state
- NVS Flash integration for WiFi credential storage
- Comprehensive API documentation
- Basic example project
- CMake-based build system with static library distribution
- Support for ESP32, ESP32-S2, ESP32-S3, ESP32-C2, ESP32-C3, ESP32-C6

### Changed

- N/A (initial release)

### Deprecated

- N/A

### Removed

- N/A

### Fixed

- N/A

### Security

- ECDH key exchange prevents MITM attacks
- AES-GCM authenticated encryption for WiFi credentials
- Configurable device anonymization options

## [0.9.0-beta] - 2025-01-XX

### Added

- Private beta release for testing
- Core provisioning functionality
- BLE characteristic framework
- WiFi connectivity validation

### Known Issues

- iOS provisioning app requires CocoaPods setup
- Android support pending
- Some edge cases in WiFi reconnection
