#pragma once

#include "widgets/ClockCard.h"
#include "widgets/CameraCard.h"


class UI
{

private:

    ClockCard clock;
    CameraCard camera;


public:

    void init();

    void update();

};
