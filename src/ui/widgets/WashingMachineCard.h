#pragma once

#include <lvgl.h>


// Card dedicata per la lavatrice: icona disegnata con forme LVGL
// (nessun font esterno), sfondo verde/rosso in base alla potenza
// assorbita, valore di potenza al posto dell'etichetta di testo.
// Il tap sulla card accende/spegne switch.fvbalcone_presa_1.
class WashingMachineCard
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
    lv_obj_t* iconBody = nullptr;
    lv_obj_t* iconDrum = nullptr;
    lv_obj_t* iconDoorLock = nullptr;
    lv_obj_t* valueLabel = nullptr;

    lv_timer_t* updateTimer = nullptr;

    static void onClick(lv_event_t* e);  // disabilitato su .cpp per non spegnere per sbaglio
    static void timerCallback(lv_timer_t* timer);

};
