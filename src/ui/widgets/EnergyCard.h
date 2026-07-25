#pragma once
#include <lvgl.h>
#include "../../data/EnergyData.h"
#define ENERGY_CARD_WIDTH  1080
#define ENERGY_CARD_HEIGHT 715


class EnergyCard
{

public:

    void create(
        lv_obj_t* parent,
        int x,
        int y
    );


    void update(
        const EnergyData& data
    );


private:

    lv_obj_t* container;

    lv_obj_t* image;


    lv_obj_t* lblPowerFlow;

    lv_obj_t* lblPV;

    lv_obj_t* lblBattery;

    lv_obj_t* lblOutside;

    lv_obj_t* lblLiving;

    lv_obj_t* lblNight;

    lv_obj_t* lblEntrance;

    lv_obj_t* lblCar;

    lv_obj_t* lblInverter;



    lv_obj_t* createLabel(
        int x,
        int y,
        int size = 18
    );

};