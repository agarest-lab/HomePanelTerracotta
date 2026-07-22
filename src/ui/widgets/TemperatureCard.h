#pragma once

#include <lvgl.h>


class TemperatureCard
{

private:

    lv_obj_t* card = nullptr;

    lv_obj_t* temperatureLabel = nullptr;
    lv_obj_t* humidityLabel = nullptr;


    lv_obj_t* createText(
        lv_obj_t* parent,
        const char* text,
        int x,
        int y,
        int size
    );


public:

    void create();


    void setTemperature(
        float value
    );


    void setHumidity(
        float value
    );

};