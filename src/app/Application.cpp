#include "Application.h"


void Application::run()
{

    if(!display.init())
    {
        return;
    }


    while(true)
    {

        display.loop();

    }

}