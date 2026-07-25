#include "HomeScreen.h"

#include <iostream>
#include <lvgl.h>



void HomeScreen::create()
{

    energyCard.create(
        lv_screen_active(),
        0,
        350
    );


    // Prima lettura dati
    updateData();


    // Aggiornamento automatico
    updateTimer =
        lv_timer_create(
            timerCallback,
            2000,
            this
        );
}



void HomeScreen::updateData()
{

    std::cout << "Updating EnergyCard" << std::endl;
    //
    // CASA
    //

    energyData.housePower =
        homeAssistant.getFloatState(
            "sensor.potenza_somma_casa"
        );

    std::cout << "House power: "
              << energyData.housePower
              << std::endl;

    energyData.gridPower =
        homeAssistant.getFloatState(
            "sensor.contatore_di_energia_potenza_attiva"
        );


    energyData.importedTodayKwh =
        homeAssistant.getFloatState(
            "sensor.en_importata_sun_oggi"
        );


    energyData.exportedTodayKwh =
        homeAssistant.getFloatState(
            "sensor.en_immessa_sun_oggi"
        );



    //
    // FOTOVOLTAICO
    //

    energyData.pvPower =
        homeAssistant.getFloatState(
            "sensor.inverter_potenza_attiva"
        );


    energyData.pvTodayKwh =
        homeAssistant.getFloatState(
            "sensor.inverter_rendimento_giornaliero"
        );



    //
    // BATTERIA
    //

    energyData.batterySOC =
        homeAssistant.getFloatState(
            "sensor.batterie_stato_della_capacita"
        );


    energyData.batteryPower =
        homeAssistant.getFloatState(
            "sensor.batterie_potenza_di_carica_scarica"
        );


    energyData.batteryTemp =
        homeAssistant.getFloatState(
            "sensor.batteria_1_temperatura_bms"
        );



    //
    // AMBIENTE ESTERNO
    //

    energyData.outsideTemp =
        homeAssistant.getFloatState(
            "sensor.openweathermap_temperature"
        );


    energyData.outsideHum =
        homeAssistant.getFloatState(
            "sensor.openweathermap_humidity"
        );



    //
    // SOGGIORNO
    //

    energyData.livingTemp =
        homeAssistant.getFloatState(
            "sensor.soggiorno_soggiorno_temperatura_soggiorno"
        );


    energyData.livingHum =
        homeAssistant.getFloatState(
            "sensor.soggiorno_soggiorno_umidita_soggiorno"
        );



    //
    // CAMERA / NOTTE
    //

    energyData.nightTemp =
        homeAssistant.getFloatState(
            "sensor.camera_temperatura_camera"
        );


    energyData.nightHum =
        homeAssistant.getFloatState(
            "sensor.camera_umidita_camera"
        );



    //
    // INGRESSO
    //

    energyData.entranceTemp =
        homeAssistant.getFloatState(
            "sensor.temp_ingresso"
        );



    //
    // INVERTER
    //

    energyData.inverterPower =
        homeAssistant.getFloatState(
            "sensor.inverter_potenza_attiva"
        );


    energyData.inverterTemp =
        homeAssistant.getFloatState(
            "sensor.inverter_internal_temperature"
        );



    //
    // AUTO ELETTRICA
    //

    energyData.carSOC =
        homeAssistant.getFloatState(
            "sensor.saic_mg_mg4_soc"
        );


    energyData.carPower =
        homeAssistant.getFloatState(
            "sensor.1p7k_new_potenza"
        );



    //
    // Aggiorna interfaccia
    //

    energyCard.update(
        energyData
    );


    std::cout
        << "EnergyCard updated"
        << std::endl;
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