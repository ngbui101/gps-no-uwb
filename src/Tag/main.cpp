#include "Device.h"

int count = 0;
void setup()
{
    Serial.begin(115200);
    delay(3000);
    Serial.println("This is UWB debug serial!");
    Serial.print("Device: ");
    Serial.println(DEVICE_NAME);
    delay(1000);
    if (!Device::getInstance().begin())
    {
        while (true)
            ;
    }
}

void loop()
{
    // Serial.println(count);
    // count++;
    // delay(1000);
    Device::getInstance().run_tag();
}
