#pragma once

#include <lvgl.h>


class ClockCard
{

public:

    void create(
        lv_obj_t* parent,
        int x,
        int y
    );

    void update();


private:

    lv_obj_t* card = nullptr;

    lv_obj_t* timeLabel = nullptr;
    lv_obj_t* dateLabel = nullptr;
    lv_obj_t* extLabel = nullptr;
    lv_timer_t* updateTimer = nullptr;

    static void timerCallback(lv_timer_t* timer);

};