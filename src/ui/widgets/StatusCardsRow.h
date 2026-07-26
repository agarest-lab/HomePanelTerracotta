#pragma once

#include <lvgl.h>


// Riga di 4 card affiancate, larghe quanto lo schermo, che mostrano:
// - Limite anticondensa (valore singolo)
// - ACS (due valori, sfondo verde/rosso in base a una soglia)
// - Impianto (due valori)
// - Stato allarme (testo + sfondo colorato in base allo stato)
class StatusCardsRow
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

    lv_obj_t* condensaCard = nullptr;
    lv_obj_t* condensaValueLabel = nullptr;

    lv_obj_t* acsCard = nullptr;
    lv_obj_t* acsPdcLabel = nullptr;
    lv_obj_t* acsCasaLabel = nullptr;

    lv_obj_t* impiantoCard = nullptr;
    lv_obj_t* mandataLabel = nullptr;
    lv_obj_t* ritornoLabel = nullptr;

    lv_obj_t* alarmCard = nullptr;
    lv_obj_t* alarmLabel = nullptr;

    lv_timer_t* updateTimer = nullptr;

    static void timerCallback(lv_timer_t* timer);

    lv_obj_t* createCardContainer(
        lv_obj_t* parent,
        int x,
        int y,
        int w,
        int h
    );

    lv_obj_t* createValueLabel(
        lv_obj_t* parent,
        int x,
        int y,
        lv_color_t color
    );

};
