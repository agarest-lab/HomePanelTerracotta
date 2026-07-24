#include "HomeScreen.h"
#include <lvgl.h>
#include <cstdio>
#include "../widgets/EnergyCard.h"
#include "../../data/EnergyData.h"

EnergyCard energyCard;

void HomeScreen::create()
{

    energyCard.create(
        lv_screen_active(),
        0,
        350
    );


    EnergyData data;

    energyData.housePower =
    homeAssistant.getFloatState(
        "sensor.contatore_di_energia_potenza_attiva"
    );


    energyData.pvPower =
        homeAssistant.getFloatState(
            "sensor.inverter_potenza_attiva"
        );


    energyData.livingTemp =
         homeAssistant.getFloatState(
            "sensor.soggiorno_soggiorno_temperatura_soggiorno"
        );


    energyData.livingHum =
        homeAssistant.getFloatState(
            "sensor.soggiorno_soggiorno_umidita_soggiorno"
        );


    energyCard.update(
        energyData
    );

    energyCard.update(energyData);

}