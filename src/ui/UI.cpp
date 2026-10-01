#include "UI.h"
#include "../ui/screens/HomeScreen.h"


static HomeScreen homeScreen;


void UI::init()
{
    // Tutta la schermata vive in HomeScreen: ogni pannello si aggiorna
    // da solo con un timer LVGL.
    homeScreen.create();
}


void UI::update()
{
    // Niente da fare: i pannelli si aggiornano autonomamente.
}
