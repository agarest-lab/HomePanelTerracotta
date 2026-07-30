#include "Application.h"
#include "../services/HomeAssistantRepository.h"

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


    // Avvia i thread di polling/comandi verso Home Assistant.
    // Va chiamato DOPO ui.init(), che registra (watch) tutte le entità
    // necessarie tramite HomeScreen::create() e UI::init().
    HomeAssistantRepository::instance().start();


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

    // Non raggiunto nel loop infinito attuale; se in futuro aggiungi
    // una condizione di uscita, ricordati di fermare il repository:
    // HomeAssistantRepository::instance().stop();
}
