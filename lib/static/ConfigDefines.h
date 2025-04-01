#ifndef CONFIG_DEFINES_H
#define CONFIG_DEFINES_H
#ifdef TAG
#define DEVICE_NAME "UWB_TAG"
#elif defined(ANCHOR_1)
#define DEVICE_NAME "ANCHOR_1"
#elif defined(ANCHOR_2)
#define DEVICE_NAME "ANCHOR_2"
#elif defined(ANCHOR_3)
#define DEVICE_NAME "ANCHOR_3"
#endif

#define DEVICE_HEARTBEAT_INTERVAL 60000

#define WIFI_SSID "TestWlan"
#define WIFI_PASSWORD "123456789"
#define WIFI_AUTO_RECONNECT true
#define WIFI_CHECK_INTERVAL 500
#define WIFI_RECONNECT_INTERVAL 5000
#define WIFI_MAX_CONNECTION_ATTEMPTS 20
#define WIFI_FTM_FRAME_COUNT 16
#define WIFI_FTM_BURST_PERIOD 2

#define MQTT_BROKER "mathadventure.hs-bochum.de"
#define MQTT_PORT 1883
#define MQTT_USER "gpsno"
#define MQTT_PASSWORD "S%&n@b5LHnog4tV5"
#define MQTT_RETRY_INTERVAL 5000
#define MQTT_MAX_CONNECTION_ATTEMPTS 20
#define MQTT_BASE_TOPIC "gpsno/devices"

#define BLUETOOTH_TIMEOUT 5000
#define BLUETOOTH_MAX_CONNECTIONS 3

#define ERROR_MAX_RECOVERY_ATTEMPTS 3
#define ERROR_RECOVERY_INTERVAL 5000

#define LOGGING_LEVEL 0 // 0: DEBUG, 1: INFO, 2: WARNING, 3: ERROR
#define LOGGING_ALLOW_MQTT_LOG true
#define LOGGING_MQTT_TOPIC ""

// #define UPDATE_GITHUB_API_URL "https://api.github.com/repos/Legincy/gps-no-fw/releases/latest"
// #define UPDATE_GITHUB_API_TOKEN ""
// #define UPDATE_INTERVAL 10000
// #define UPDATE_INITIAL_CHECK true

#define DEBUG_FORCE_CONFIG true
#endif