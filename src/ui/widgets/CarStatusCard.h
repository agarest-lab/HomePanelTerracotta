#pragma once

#include <lvgl.h>
#include <string>


class CarStatusCard
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
    lv_obj_t* socLabel = nullptr;
    lv_obj_t* energyLabel = nullptr;
    lv_obj_t* dateLabel = nullptr;

    lv_timer_t* updateTimer = nullptr;

    static void timerCallback(lv_timer_t* timer);

    // Estrae "dd/mm HH:MM" da una stringa ISO tipo
    // "2026-07-27T18:30:00+00:00". Se il formato non è quello atteso,
    // ritorna la stringa originale così com'è.
    static std::string formatDateTime(const std::string& raw);

};
