#include "UI.h"

#include <lvgl.h>


void UI::init()
{

    lv_obj_t* label;


    label = lv_label_create(
        lv_screen_active()
    );


    lv_label_set_text(
        label,
        "HomePanel"
    );


    lv_obj_center(label);

}