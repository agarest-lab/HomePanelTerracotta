#include "EnergyCard.h"

#include <lvgl.h>
#include <cstdio>


extern const lv_image_dsc_t test_image;


void EnergyCard::create(
    lv_obj_t* parent,
    int x,
    int y
)
{
    printf("ENERGY CARD CREATE\n");


    lv_obj_t* img = lv_image_create(parent);


    lv_image_set_src(
        img,
        &test_image
    );


    lv_obj_set_pos(
        img,
        x,
        y
    );


    printf("IMAGE CREATED\n");
}