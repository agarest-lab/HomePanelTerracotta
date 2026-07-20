#pragma once

#include "SDLDisplay.h"


class Display
{

private:

    SDLDisplay backend;


public:

    bool init();

    void loop();

};