#pragma once

#include "../widgets/EnergyCard.h"
#include "../../data/EnergyData.h"
#include "../../services/HomeAssistantClient.h"


class HomeScreen
{

public:

    void create();


private:

    EnergyData energyData;

    EnergyCard energyCard;


    HomeAssistantClient homeAssistant;

};