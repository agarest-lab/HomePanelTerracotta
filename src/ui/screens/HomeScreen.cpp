#include "HomeScreen.h"
#include "../../services/HomeAssistantRepository.h"

#include <lvgl.h>


void HomeScreen::create()
{

    lv_obj_set_style_bg_color(lv_screen_active(), lv_color_black(), 0);
    lv_obj_set_style_bg_opa(lv_screen_active(), LV_OPA_COVER, 0);


    motionLeds.create(
        lv_screen_active(),
        650,
        305,
        {
            { "binary_sensor.ezviz_esterna_movimento_2", "Esterna" },
            { "binary_sensor.ezviz_terrazzo_movimento",  "Terrazzo" },
            { "binary_sensor.cam_garage_movimento_2",    "Garage" },
        }
    );

    // Gauge energia: sostituisce EnergyCard, subito sotto clock/camera
    // (che restano creati altrove, es. in UI::init(), come nel branch precedente)
    energyGauges.create(
        lv_screen_active(),
        0,
        390,
        1080
    );

    lightsRow.create(lv_screen_active(), 0, 780, {
        { "switch.led_cucina_switch_0",          "switch", "Led cucina" },
        { "switch.penisola_switch_0",            "switch", "Penisola" },
        { "switch.deviatore_wi_fi_ble_switch_1", "switch", "Soggiorno" },
        { "switch.luce_dispensa_switch_0",       "switch", "Dispensa" },
        { "switch.luce_armadio_switch_0",        "switch", "Armadio" },
    });
    
    socketsRow.create(lv_screen_active(), 0, 920, {
        { "switch.lampada_presa",         "switch", "Lampada" },
        { "switch.luce_garage_switch_0",  "switch", "Garage" },
        { "switch.luce_scala_switch_0",   "switch", "Scala" },
        { "light.cuoricini",              "light",  "Cuoricini" },
    });
    
    washingMachine.create(
        lv_screen_active(),
        27 + 4 * (200 + 13),   // x del 5° slot della riga (stessi TOGGLE_WIDTH/GAP di ToggleRow)
        920,                   // <-- stessa Y usata per socketsRow.create(...)
        195,
        120                    // stesse dimensioni dei toggle
    );

    statusRow.create(
        lv_screen_active(),
        0,
        1060,
        1080,
        150
    );

    weatherRow.create(
        lv_screen_active(),
        0,
        1230,
        1080,
        150
    );

    dayNightRow.create(
        lv_screen_active(),
        0,
        1400,
        715,
        150
    );

    carStatusCard.create(
        lv_screen_active(),
        730,
        1400,     // <-- sotto dayNightRow, verifica tu la posizione esatta
        350,
        150      // altezza indicativa
    );

    calendarCard.create(
        lv_screen_active(),
        0,
        1560,     // <-- sotto tutto il resto, verifica tu la posizione
        1080,
        360
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
