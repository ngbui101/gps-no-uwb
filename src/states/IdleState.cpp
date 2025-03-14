#include "states/IdleState.h"

void IdleState::enter()
{
    log.debug("IdleState", "Entering IdleState");
}

void IdleState::update()
{
    device->changeState(SetupState::getInstance(device));
}

void IdleState::exit()
{
    log.debug("IdleState", "Exiting IdleState");
}