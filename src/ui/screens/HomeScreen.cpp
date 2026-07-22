#include "HomeScreen.h"
#include <lvgl.h>
#include <cstdio>
#include "../widgets/EnergyCard.h"

void HomeScreen::create()
{
    printf("HOMESCREEN CREATE\n");
    lv_obj_t* screen = lv_screen_active();

    lv_obj_set_style_bg_color(
        screen,
        lv_color_hex(0x101010),
        0
    );

    EnergyCard::create(
        screen,
        10,
        350
    );

    lv_obj_t* title = lv_label_create(screen);

    lv_label_set_text(
        title,
        "HomePanel"
    );


    lv_obj_set_style_text_color(
        title,
        lv_color_hex(0xffffff),
        0
    );


    lv_obj_set_style_text_font(
        title,
        &lv_font_montserrat_14,
        0
    );


    lv_obj_align(
        title,
        LV_ALIGN_TOP_MID,
        0,
        30
    );

}