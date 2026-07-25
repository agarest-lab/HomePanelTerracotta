#include "UI.h"
#include "../ui/screens/HomeScreen.h"


static HomeScreen homeScreen;


void UI::init()
{
    homeScreen.create();

    clock.create(lv_screen_active(), 20, 20);
}


void UI::update()
{
    // Il ClockCard si aggiorna da solo tramite il proprio timer interno,
    // quindi qui non serve più fare nulla legato all'orario.
}