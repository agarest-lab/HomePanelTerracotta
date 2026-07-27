#pragma once

#include "../widgets/EnergyCard.h"
#include "../widgets/EntityButtonRow.h"
#include "../widgets/StatusCardsRow.h"
#include "../../data/EnergyData.h"
#include "../widgets/WeatherCardsRow.h"


class HomeScreen
{

public:

    void create();
    void updateData();


private:

    static void timerCallback(
        lv_timer_t* timer
    );

    lv_timer_t* updateTimer = nullptr;

    EnergyCard energyCard;
    EnergyData energyData;

    EntityButtonRow lightsRow;
    EntityButtonRow socketsRow;

    StatusCardsRow statusRow;
    WeatherCardsRow weatherRow;

};
