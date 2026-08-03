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

    // vertical = true: i LED vengono impilati in colonna invece che
    // affiancati in riga (utile per spazi stretti, es. tra due card).
    void create(
        lv_obj_t* parent,
        int x,
        int y,
        const std::vector<MotionSensorConfig>& configs,
        bool vertical = false
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
