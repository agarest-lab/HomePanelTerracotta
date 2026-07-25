#pragma once

struct EnergyValue
{
    float value;
    bool valid;
};

struct EnergyData
{
    // Casa
    float housePower = 0;
    float gridPower = 0;
    float importedTodayKwh = 0;
    float exportedTodayKwh = 0;


    // Fotovoltaico
    float pvPower = 0;
    float pvTodayKwh = 0;


    // Batteria
    float batterySOC = 0;
    float batteryPower = 0;
    float batteryTemp = 0;


    // Temperature ambiente
    float outsideTemp = 0;
    float outsideHum = 0;

    float livingTemp = 0;
    float livingHum = 0;

    float nightTemp = 0;
    float nightHum = 0;

    float entranceTemp = 0;


    // Inverter
    float inverterPower = 0;
    float inverterTemp = 0;


    // Auto
    float carSOC = 0;
    float carPower = 0;
};
