#pragma once

#include <lvgl.h>
#include <memory>
#include <string>

#include "../../services/CameraFeed.h"


class CameraCard
{

public:

    // width/height definiscono il riquadro in cui l'immagine viene
    // scalata mantenendo le proporzioni originali.
    void create(
        lv_obj_t* parent,
        int x,
        int y,
        int width,
        int height,
        const std::string& entityId
    );

    // Controlla se è disponibile un nuovo frame e, in caso, lo mostra.
    void refresh();


private:

    lv_obj_t* card = nullptr;
    lv_obj_t* image = nullptr;

    int boxWidth = 0;
    int boxHeight = 0;

    lv_image_dsc_t imgDsc{};

    // Tiene in vita il buffer di pixel finché è quello mostrato a schermo
    // (LVGL referenzia il puntatore direttamente, non lo copia).
    std::shared_ptr<CameraFrame> currentFrame;

    lv_timer_t* updateTimer = nullptr;

    static void timerCallback(lv_timer_t* timer);

};
