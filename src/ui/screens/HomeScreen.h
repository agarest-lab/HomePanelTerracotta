#pragma once

#include "../widgets/EnergyGaugesPanel.h"
#include "../widgets/ToggleRow.h"
#include "../widgets/StatusCardsRow.h"
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

    EnergyGaugesPanel energyGauges;
    ToggleRow lightsRow;
    ToggleRow socketsRow;
    StatusCardsRow statusRow;
    WeatherCardsRow weatherRow;

};
