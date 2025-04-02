#ifndef CONFIG_DEFINES_H
#define CONFIG_DEFINES_H

// UWB
// total of devices 1 TAG + (NUM_DEVS - 1) ANCHORS, Min 2, max 5
#define NUM_DEVS 2
// PIN
#define UWB_RST 27
#define UWB_IRQ 34
#define UWB_SS 4
// FPS
#define INTERVAL 10
// Antenna delay
#define TX_ANT_DLY 16385
#define RX_ANT_DLY 16385
// ID
#define TAG_ID 0
#define ANCHORID_1 4
#define ANCHORID_2 8
#define ANCHORID_3 12
// Tag
#ifdef TAG
#define DEVICE_NAME "UWB_TAG"
#define UID TAG_ID
#define QUEUE_NUM 0
#elif defined(ANCHOR_1)
#define DEVICE_NAME "ANCHOR_1"
#define UID ANCHORID_1
#define QUEUE_NUM_NUM 1
#elif defined(ANCHOR_2)
#define DEVICE_NAME "ANCHOR_2"
#define UID ANCHORID_2
#define QUEUE_NUM_NUM 2
#elif defined(ANCHOR_3)
#define DEVICE_NAME "ANCHOR_3"
#define UID ANCHORID_3
#define QUEUE_NUM_NUM 3
#else
#define DEVICE_NAME "NULL"
#define UID TAG_ID
#define WAIT_NUM 0
#endif

#define DEVICE_HEARTBEAT_INTERVAL 60000
// Wlan
#define WIFI_SSID "TestWlan1"
#define WIFI_PASSWORD "123456789test"
#define WIFI_AUTO_RECONNECT true
#define WIFI_CHECK_INTERVAL 500
#define WIFI_RECONNECT_INTERVAL 5000
#define WIFI_MAX_CONNECTION_ATTEMPTS 20
#define WIFI_FTM_FRAME_COUNT 16
#define WIFI_FTM_BURST_PERIOD 2
// MQTT
#define MQTT_BROKER_ADDRESS "192.168.89.50"
#define MQTT_PORT 1883
#define MQTT_USER "test_user"
#define MQTT_PASSWORD "123456789"
#define MQTT_RETRY_INTERVAL 5000
#define MQTT_MAX_CONNECTION_ATTEMPTS 20
#define MQTT_BASE_TOPIC "device"
#define MQTT_KEEP_ALIVE 180
#define MQTT_QOS 0
// Bluetooth
#define BLUETOOTH_TIMEOUT 5000
#define BLUETOOTH_MAX_CONNECTIONS 3
// Recovery
#define ERROR_MAX_RECOVERY_ATTEMPTS 3
#define ERROR_RECOVERY_INTERVAL 5000
// LOG
#define LOGGING_LEVEL 0 // 0: DEBUG, 1: INFO, 2: WARNING, 3: ERROR
#define LOGGING_ALLOW_MQTT_LOG true
#define LOGGING_MQTT_TOPIC ""

// #define UPDATE_GITHUB_API_URL "https://api.github.com/repos/Legincy/gps-no-fw/releases/latest"
// #define UPDATE_GITHUB_API_TOKEN ""
// #define UPDATE_INTERVAL 10000
// #define UPDATE_INITIAL_CHECK true

#define DEBUG_FORCE_CONFIG true
#endif