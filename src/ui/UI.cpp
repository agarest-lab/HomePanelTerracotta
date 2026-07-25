#include "UI.h"
#include "../ui/screens/HomeScreen.h"

#include <iostream>


static HomeScreen homeScreen;


void UI::init()
{
    homeScreen.create();

    // Metà sinistra, in alto: orologio + data
    clock.create(lv_screen_active(), 20, 20);

    // Metà destra, in alto: immagine telecamera esterna
    camera.create(
        lv_screen_active(),
        560,   // x: subito dopo il ClockCard (che occupa 20..560)
        20,    // y
        500,   // larghezza riquadro
        280,   // altezza riquadro
        "camera.ezviz_esterna"
    );
}


void UI::update()
{
    // ClockCard e CameraCard si aggiornano da soli tramite i propri
    // timer interni, quindi qui non serve fare nulla.
}
