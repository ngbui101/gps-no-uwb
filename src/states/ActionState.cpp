#include "states/ActionState.h"

void ActionState::enter()
{
    log.debug("ActionState", "Entering ActionState");
}

void ActionState::update()
{
    MQTTManager::getInstance().update();
}

void ActionState::exit()
{
    log.debug("ActionState", "Exiting ActionState");
}
