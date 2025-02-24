#include "states/TestState.h"

void TestState::enter(){
    log.debug("TestState", "Entering TestState");
}

void TestState::update(){
    device->changeState(TestState::getInstance(device));
}

void TestState::exit(){
    log.debug("TestState", "Exiting TestState");
}