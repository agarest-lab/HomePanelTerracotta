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


    data.housePower = 420;
    data.gridPower = 80;

    data.batterySOC = 85;
    data.batteryPower = 2300;
    data.batteryTemp = 31;

    data.outsideTemp = 24.1;
    data.outsideHum = 58;

    data.livingTemp = 22.8;
    data.livingHum = 51;

    data.carSOC = 72;
    data.carPower = 7400;


    energyCard.update(data);

}