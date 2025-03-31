#include "Device.h"

void setup()
{
    Serial.begin(115200);
    delay(3000);
    Serial.println("This is UWB debug serial!!!");
    delay(1000);
    if (!Device::getInstance().begin())
    {
        while (true)
            ;
    }
}

void loop()
{
}
