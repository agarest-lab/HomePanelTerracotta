#pragma once

#include <lvgl.h>


// Pannello con 4 gauge ad arco (Casa, Fotovoltaico, Batteria, Rete)
// che sostituisce l'immagine di sfondo di EnergyCard con widget LVGL nativi.
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
        lv_obj_t* subLabel = nullptr;
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
        lv_color_t arcColor
    );

};
