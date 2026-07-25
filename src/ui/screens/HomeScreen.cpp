#include "HomeScreen.h"
#include "../../services/HomeAssistantRepository.h"

#include <iostream>
#include <lvgl.h>


void HomeScreen::create()
{
    lv_obj_set_style_bg_color(lv_screen_active(), lv_color_black(), 0);
    lv_obj_set_style_bg_opa(lv_screen_active(), LV_OPA_COVER, 0);
    
    energyCard.create(
        lv_screen_active(),
        0,
        467
    );

    lightsRow.create(lv_screen_active(), 27, 1190, {
        { "switch.led_cucina_switch_0",          "switch", "Led cucina" },
        { "switch.penisola_switch_0",            "switch", "Penisola" },
        { "switch.deviatore_wi_fi_ble_switch_1", "switch", "Soggiorno" },
        { "switch.luce_dispensa_switch_0",       "switch", "Dispensa" },
        { "switch.luce_armadio_switch_0",        "switch", "Armadio" },
    });

    socketsRow.create(lv_screen_active(), 27, 1323, {
        { "switch.lampada_presa",         "switch", "Lampada" },
        { "switch.luce_garage_switch_0",  "switch", "Garage" },
        { "switch.luce_scala_switch_0",   "switch", "Scala" },
        { "light.cuoricini",              "light",  "Cuoricini" },
        { "switch.fvbalcone_presa_1",     "switch", "Lavatrice" },
    });

    // Registra presso il repository tutte le entità sensore usate
    // dalla EnergyCard, così il thread di polling in background
    // inizia a tenerle aggiornate.
    auto& repo = HomeAssistantRepository::instance();

    repo.watch("sensor.potenza_somma_casa");
    repo.watch("sensor.contatore_di_energia_potenza_attiva");
    repo.watch("sensor.en_importata_sun_oggi");
    repo.watch("sensor.en_immessa_sun_oggi");

    repo.watch("sensor.inverter_potenza_attiva");
    repo.watch("sensor.inverter_rendimento_giornaliero");

    repo.watch("sensor.batterie_stato_della_capacita");
    repo.watch("sensor.batterie_potenza_di_carica_scarica");
    repo.watch("sensor.batteria_1_temperatura_bms");

    repo.watch("sensor.openweathermap_temperature");
    repo.watch("sensor.openweathermap_humidity");

    repo.watch("sensor.soggiorno_soggiorno_temperatura_soggiorno");
    repo.watch("sensor.soggiorno_soggiorno_umidita_soggiorno");

    repo.watch("sensor.camera_temperatura_camera");
    repo.watch("sensor.camera_umidita_camera");

    repo.watch("sensor.temp_ingresso");

    repo.watch("sensor.inverter_internal_temperature");

    repo.watch("sensor.saic_mg_mg4_soc");
    repo.watch("sensor.1p7k_new_potenza");


    // Prima lettura (dalla cache, sarà vuota finché il thread di
    // polling non avrà completato il primo giro)
    updateData();


    // Aggiornamento periodico della UI (legge solo dalla cache, veloce)
    updateTimer =
        lv_timer_create(
            timerCallback,
            2000,
            this
        );
}


void HomeScreen::updateData()
{
    auto& repo = HomeAssistantRepository::instance();

    //
    // CASA
    //

    energyData.housePower = repo.getFloatState("sensor.potenza_somma_casa");
    energyData.gridPower  = repo.getFloatState("sensor.contatore_di_energia_potenza_attiva");

    energyData.importedTodayKwh = repo.getFloatState("sensor.en_importata_sun_oggi");
    energyData.exportedTodayKwh = repo.getFloatState("sensor.en_immessa_sun_oggi");


    //
    // FOTOVOLTAICO
    //

    energyData.pvPower    = repo.getFloatState("sensor.inverter_potenza_attiva");
    energyData.pvTodayKwh = repo.getFloatState("sensor.inverter_rendimento_giornaliero");


    //
    // BATTERIA
    //

    energyData.batterySOC   = repo.getFloatState("sensor.batterie_stato_della_capacita");
    energyData.batteryPower = repo.getFloatState("sensor.batterie_potenza_di_carica_scarica");
    energyData.batteryTemp  = repo.getFloatState("sensor.batteria_1_temperatura_bms");


    //
    // AMBIENTE ESTERNO
    //

    energyData.outsideTemp = repo.getFloatState("sensor.openweathermap_temperature");
    energyData.outsideHum  = repo.getFloatState("sensor.openweathermap_humidity");


    //
    // SOGGIORNO
    //

    energyData.livingTemp = repo.getFloatState("sensor.soggiorno_soggiorno_temperatura_soggiorno");
    energyData.livingHum  = repo.getFloatState("sensor.soggiorno_soggiorno_umidita_soggiorno");


    //
    // CAMERA / NOTTE
    //

    energyData.nightTemp = repo.getFloatState("sensor.camera_temperatura_camera");
    energyData.nightHum  = repo.getFloatState("sensor.camera_umidita_camera");


    //
    // INGRESSO
    //

    energyData.entranceTemp = repo.getFloatState("sensor.temp_ingresso");


    //
    // INVERTER
    //

    energyData.inverterPower = repo.getFloatState("sensor.inverter_potenza_attiva");
    energyData.inverterTemp  = repo.getFloatState("sensor.inverter_internal_temperature");


    //
    // AUTO ELETTRICA
    //

    energyData.carSOC   = repo.getFloatState("sensor.saic_mg_mg4_soc");
    energyData.carPower = repo.getFloatState("sensor.1p7k_new_potenza");


    //
    // Aggiorna interfaccia (solo lettura da cache, mai bloccante)
    //

    energyCard.update(energyData);

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
