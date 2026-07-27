#pragma once

#include <lvgl.h>
#include <vector>
#include <string>


class CalendarCard
{

public:

    void create(
        lv_obj_t* parent,
        int x,
        int y,
        int width,
        int height
    );

    void refresh();


private:

    lv_obj_t* card = nullptr;
    lv_obj_t* listContainer = nullptr;

    int maxEvents = 6;

    lv_timer_t* updateTimer = nullptr;

    static void timerCallback(lv_timer_t* timer);

    // Colore per calendario, in base all'entity_id (stesso concetto del
    // campo "color" nello YAML originale della dashboard)
    static lv_color_t colorForCalendar(const std::string& entityId);

};
