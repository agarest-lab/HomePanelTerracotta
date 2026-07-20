#include "Display.h"


bool Display::init()
{
    return backend.init();
}


void Display::loop()
{
    backend.loop();
}