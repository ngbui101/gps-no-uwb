#ifndef CONFIG_DEFINES_H
#define CONFIG_DEFINES_H
#define DEVICE_NAME "DW3000"
// UWB
// total of devices: 1 TAG + (NUM_DW3000 - 1) ANCHORS, Min 2, max 5
#define NUM_DW3000 3
#define INTERVAL 5 /* MAX FPS = 1000 / INTERVAL */

#define U0 0
#define U1 14
#define U2 18
#define U3 22
#define U4 26
#define U5 30
#define U6 34

#ifdef TAG
#define DEVICE_NAME "TAG"
#define UID U1
#define TX_ANT_DLY 16385
#define RX_ANT_DLY 16385
#define WAIT_NUM 0
#elif defined(ANCHOR_U1)
#define DEVICE_NAME "ANCHOR_U1"
#define UID U2
#define TX_ANT_DLY 16385
#define RX_ANT_DLY 16385
#define WAIT_NUM 1
#elif defined(ANCHOR_U2)
#define DEVICE_NAME "ANCHOR_U2"
#define UID U3
#define TX_ANT_DLY 16385
#define RX_ANT_DLY 16385
#define WAIT_NUM 2
#elif defined(ANCHOR_U3)
#define DEVICE_NAME "ANCHOR_U3"
#define UID U4
#define TX_ANT_DLY 16385
#define RX_ANT_DLY 16385
#define WAIT_NUM 3
#elif defined(ANCHOR_U4)
#define DEVICE_NAME "ANCHOR_U4"
#define UID U5
#define TX_ANT_DLY 16385
#define RX_ANT_DLY 16385
#define WAIT_NUM 4
#elif defined(ANCHOR_U5)
#define DEVICE_NAME "ANCHOR_U5"
#define UID U6
#define TX_ANT_DLY 16385
#define RX_ANT_DLY 16385
#define WAIT_NUM 5
#else
#define DEVICE_NAME "DEVICE UNKNOWN"
#define UID U0
#define TX_ANT_DLY 16385
#define RX_ANT_DLY 16385
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
#define MQTT_BROKER_ADDRESS "test.mosquitto.org"
#define MQTT_PORT 1883
#define MQTT_USER ""
#define MQTT_PASSWORD ""
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