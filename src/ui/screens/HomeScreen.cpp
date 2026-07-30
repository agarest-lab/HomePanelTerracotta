#include "HomeScreen.h"
#include "../../services/HomeAssistantRepository.h"

#include <lvgl.h>


void HomeScreen::create()
{

    lv_obj_set_style_bg_color(lv_screen_active(), lv_color_black(), 0);
    lv_obj_set_style_bg_opa(lv_screen_active(), LV_OPA_COVER, 0);


    // Gauge energia: sostituisce EnergyCard, subito sotto clock/camera
    // (che restano creati altrove, es. in UI::init(), come nel branch precedente)
    energyGauges.create(
        lv_screen_active(),
        0,
        320,
        1080
    );

    lightsRow.create(lv_screen_active(), 27, 800, {
        { "switch.led_cucina_switch_0",          "switch", "Led cucina" },
        { "switch.penisola_switch_0",            "switch", "Penisola" },
        { "switch.deviatore_wi_fi_ble_switch_1", "switch", "Soggiorno" },
        { "switch.luce_dispensa_switch_0",       "switch", "Dispensa" },
        { "switch.luce_armadio_switch_0",        "switch", "Armadio" },
    });
    
    socketsRow.create(lv_screen_active(), 27, 940, {
        { "switch.lampada_presa",         "switch", "Lampada" },
        { "switch.luce_garage_switch_0",  "switch", "Garage" },
        { "switch.luce_scala_switch_0",   "switch", "Scala" },
        { "light.cuoricini",              "light",  "Cuoricini" },
        { "switch.fvbalcone_presa_1",     "switch", "Lavatrice" },
    });
    
    statusRow.create(
        lv_screen_active(),
        0,
        1080,
        1080,
        150
    );

    weatherRow.create(
        lv_screen_active(),
        0,
        1250,
        1080,
        150
    );


    updateTimer =
        lv_timer_create(
            timerCallback,
            3000,
            this
        );
}


void HomeScreen::updateData()
{
    // energyGauges, lightsRow, socketsRow, statusRow, weatherRow
    // si aggiornano da soli tramite i rispettivi timer interni.
    // Questo timer resta come punto di estensione futuro se servirà
    // un aggiornamento coordinato.
    lightsRow.refresh();
    socketsRow.refresh();
    
}


void HomeScreen::timerCallback(
    lv_timer_t* timer
)
{
    HomeScreen* screen =
        static_cast<HomeScreen*>(
            lv_timer_get_user_data(timer)
        );

    if(screen)
    {
        screen->updateData();
    }
}
