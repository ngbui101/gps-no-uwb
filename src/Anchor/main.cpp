#include <Arduino.h>
#include <Wire.h>
#include "esp_core_dump.h"
#include "managers/LogManager.h"

void setup()
{
    esp_log_level_set("*", ESP_LOG_VERBOSE);
    esp_core_dump_init();

    Serial.begin(115200);
}

void loop()
{
}
