#pragma once

#include <lvgl.h>


// Riga con 2 card: "Giorno" (soggiorno) e "Notte" (camera),
// ciascuna con temperatura e umidità.
class DayNightCardsRow
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

    lv_obj_t* dayCard = nullptr;
    lv_obj_t* dayTempLabel = nullptr;
    lv_obj_t* dayHumLabel = nullptr;

    lv_obj_t* nightCard = nullptr;
    lv_obj_t* nightTempLabel = nullptr;
    lv_obj_t* nightHumLabel = nullptr;

    lv_timer_t* updateTimer = nullptr;

    static void timerCallback(lv_timer_t* timer);

    lv_obj_t* createCardContainer(
        lv_obj_t* parent,
        int x,
        int y,
        int w,
        int h,
        const char* title
    );

};
