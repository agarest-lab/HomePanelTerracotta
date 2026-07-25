#pragma once

#include "../widgets/EnergyCard.h"
#include "../../data/EnergyData.h"
#include "../../services/HomeAssistantClient.h"

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
    HomeAssistantClient homeAssistant;

};