#pragma once

#include <lvgl.h>
#include <string>
#include <vector>


struct MotionSensorConfig
{
    std::string entityId;
    std::string label;
};


class MotionLedsRow
{

public:

    void create(
        lv_obj_t* parent,
        int x,
        int y,
        const std::vector<MotionSensorConfig>& configs
    );

    void refresh();


private:

    struct LedEntry
    {
        lv_obj_t* led = nullptr;
        lv_obj_t* label = nullptr;
        MotionSensorConfig config;
    };

    std::vector<LedEntry> leds;

    lv_timer_t* updateTimer = nullptr;

    static void timerCallback(lv_timer_t* timer);

};
