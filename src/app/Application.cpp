#include "Application.h"

#include <iostream>
#include <SDL3/SDL.h>


void Application::run()
{

    std::cout << "Application start" << std::endl;


    if(!display.init())
    {
        std::cout << "Display init failed" << std::endl;
        return;
    }


    std::cout << "Display OK" << std::endl;


    ui.init();


    std::cout << "UI OK" << std::endl;



    uint32_t lastUpdate = 0;


    while(true)
    {

        display.loop();


        uint32_t now =
            SDL_GetTicks();


        if(now - lastUpdate > 5000)
        {

            ui.update();


            lastUpdate = now;

        }

    }

}