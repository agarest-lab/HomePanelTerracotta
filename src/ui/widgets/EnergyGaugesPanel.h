#pragma once

#include <lvgl.h>


class EnergyGaugesPanel
{

public:

    void create(
        lv_obj_t* parent,
        int x,
        int y,
        int width
    );

    void refresh();


private:

    struct Gauge
    {
        lv_obj_t* card = nullptr;
        lv_obj_t* arc = nullptr;
        lv_obj_t* valueLabel = nullptr;
        lv_obj_t* extraLabel = nullptr;   // riga sopra al subLabel (usata solo dalla Rete)
        lv_obj_t* subLabel = nullptr;

        // Usati solo dal gauge Casa, per il gauge interno più piccolo
        lv_obj_t* innerArc = nullptr;
        lv_obj_t* innerValueLabel = nullptr;
    };

    Gauge houseGauge;
    Gauge pvGauge;
    Gauge batteryGauge;
    Gauge gridGauge;

    lv_timer_t* updateTimer = nullptr;

    static void timerCallback(lv_timer_t* timer);

    Gauge createGauge(
        lv_obj_t* parent,
        int x,
        int y,
        int size,
        const char* title,
        lv_color_t color
    );

    void setGaugeValue(
        Gauge& gauge,
        int value,
        int maxValue,
        const char* valueText,
        const char* subText,
        lv_color_t arcColor,
        lv_color_t subTextColor
    );

};
