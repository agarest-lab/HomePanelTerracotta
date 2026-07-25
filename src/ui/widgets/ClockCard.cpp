#include "ClockCard.h"

#include <ctime>
#include <cstdio>


static const char* const kGiorni[7] = {
    "domenica", "lunedì", "martedì", "mercoledì",
    "giovedì", "venerdì", "sabato"
};

static const char* const kMesi[12] = {
    "gennaio", "febbraio", "marzo", "aprile",
    "maggio", "giugno", "luglio", "agosto",
    "settembre", "ottobre", "novembre", "dicembre"
};


void ClockCard::create(
    lv_obj_t* parent,
    int x,
    int y
)
{
    card = lv_obj_create(parent);

    lv_obj_set_size(card, 540, 280);
    lv_obj_set_pos(card, x, y);

    lv_obj_set_style_bg_opa(card, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(card, 0, 0);
    lv_obj_set_style_pad_all(card, 0, 0);


    timeLabel = lv_label_create(card);

    lv_obj_set_style_text_color(timeLabel, lv_color_white(), 0);
    lv_obj_set_style_text_font(timeLabel, &lv_font_montserrat_48, 0);

    // Serve impostare il pivot dello zoom sull'angolo in alto a sinistra,
    // altrimenti LVGL scala "dal centro" e la posizione salta.
    lv_obj_set_style_transform_pivot_x(timeLabel, 0, 0);
    lv_obj_set_style_transform_pivot_y(timeLabel, 0, 0);
    /*
    lv_obj_set_style_transform_zoom(timeLabel, 300, 0);   // 256=100%, 500≈195%
    */
    lv_obj_set_pos(timeLabel, 10, 10);


    dateLabel = lv_label_create(card);

    lv_obj_set_style_text_color(dateLabel, lv_color_white(), 0);
    lv_obj_set_style_text_font(dateLabel, &lv_font_montserrat_20, 0);

    lv_obj_set_style_transform_pivot_x(dateLabel, 0, 0);
    lv_obj_set_style_transform_pivot_y(dateLabel, 0, 0);
    /*
    lv_obj_set_style_transform_zoom(dateLabel, 250, 0);   // ≈137%
    */
    lv_obj_set_pos(dateLabel, 15, 190);


    update();

    updateTimer = lv_timer_create(timerCallback, 1000, this);
}


void ClockCard::update()
{
    std::time_t now = std::time(nullptr);
    std::tm local = *std::localtime(&now);

    char timeBuf[8];
    std::snprintf(
        timeBuf,
        sizeof(timeBuf),
        "%02d:%02d",
        local.tm_hour,
        local.tm_min
    );

    lv_label_set_text(timeLabel, timeBuf);


    char dateBuf[64];
    std::snprintf(
        dateBuf,
        sizeof(dateBuf),
        "%s %d %s",
        kGiorni[local.tm_wday],
        local.tm_mday,
        kMesi[local.tm_mon]
    );

    lv_label_set_text(dateLabel, dateBuf);
}


void ClockCard::timerCallback(lv_timer_t* timer)
{
    ClockCard* self =
        static_cast<ClockCard*>(
            lv_timer_get_user_data(timer)
        );

    if(self)
    {
        self->update();
    }
}