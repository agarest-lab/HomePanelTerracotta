#include "TemperatureCard.h"

#include <cstdio>



lv_obj_t* TemperatureCard::createText(
    lv_obj_t* parent,
    const char* text,
    int x,
    int y,
    int size
)
{
    lv_obj_t* label = lv_label_create(parent);


    lv_label_set_text(
        label,
        text
    );


    lv_obj_set_pos(
        label,
        x,
        y
    );


    lv_obj_set_style_text_color(
        label,
        lv_color_hex(0xffffff),
        0
    );


    lv_obj_set_style_text_font(
        label,
        &lv_font_montserrat_20,
        0
    );


    return label;
}




void TemperatureCard::create()
{
    card = lv_obj_create(
        lv_screen_active()
    );


    lv_obj_set_size(
        card,
        400,
        293
    );


    lv_obj_set_pos(
        card,
        53,
        160
    );


    lv_obj_set_style_bg_color(
        card,
        lv_color_hex(0x202020),
        0
    );


    lv_obj_set_style_radius(
        card,
        25,
        0
    );


    lv_obj_set_style_border_width(
        card,
        0,
        0
    );


    createText(
        card,
        "Temperatura",
        30,
        20,
        20
    );


    temperatureLabel = createText(
        card,
        "--.- C",
        80,
        60,
        20
    );


    lv_obj_set_style_text_font(
        temperatureLabel,
        &lv_font_montserrat_32,
        0
    );



    createText(
        card,
        "Umidita",
        30,
        130,
        20
    );


    humidityLabel = createText(
        card,
        "--.- %",
        80,
        165,
        20
    );


    lv_obj_set_style_text_font(
        humidityLabel,
        &lv_font_montserrat_32,
        0
    );
}





void TemperatureCard::setTemperature(
    float value
)
{
    char buffer[32];


    snprintf(
        buffer,
        sizeof(buffer),
        "%.1f C",
        value
    );


    lv_label_set_text(
        temperatureLabel,
        buffer
    );
}





void TemperatureCard::setHumidity(
    float value
)
{
    char buffer[32];


    snprintf(
        buffer,
        sizeof(buffer),
        "%.1f %%",
        value
    );


    lv_label_set_text(
        humidityLabel,
        buffer
    );
}