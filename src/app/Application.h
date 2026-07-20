#pragma once

#include "../graphics/Display.h"
#include "../ui/UI.h"


class Application
{

private:

    Display display;

    UI ui;


public:

    void run();

};