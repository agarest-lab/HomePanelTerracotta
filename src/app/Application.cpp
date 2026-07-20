#include "Application.h"
#include <iostream>

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


    while(true)
    {
        display.loop();
    }

}