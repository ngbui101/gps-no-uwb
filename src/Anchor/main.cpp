#include "Responder.h"

void setup()
{
    Serial.begin(115200);
    if (!Responder::getInstance().begin())
    {
        while (true)
            ;
    }
}

void loop()
{
    Responder::getInstance().runAnchor();
}
