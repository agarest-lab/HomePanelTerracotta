#include "EnergyCard.h"
#include "../../assets/energy_bg.h"
#define ENERGY_CARD_WIDTH  600
#define ENERGY_CARD_HEIGHT 400

lv_obj_t* EnergyCard::createLabel(
    int x,
    int y,
    int size
)
{

    lv_obj_t* label =
        lv_label_create(container);


    lv_obj_set_pos(
        label,
        x,
        y
    );


    lv_obj_set_style_text_color(
        label,
        lv_color_white(),
        0
    );


    lv_obj_set_style_text_font(
        label,
        &lv_font_montserrat_14,
        0
    );


    return label;
}



void EnergyCard::create(
    lv_obj_t* parent,
    int x,
    int y
)
{

    container =
        lv_obj_create(parent);

    lv_obj_set_size(
        container,
        ENERGY_CARD_WIDTH,
        ENERGY_CARD_HEIGHT
    );

    lv_obj_set_pos(
        container,
        x,
        y
    );

    image = lv_img_create(container);

    lv_img_set_src(
        image,
        &energy_bg
    );
    
    lv_obj_set_style_pad_all(
        image,
        0,
        0
    );


    lv_obj_set_pos(
        image,
        -26,
        -26
    );

    lblPowerFlow =
        createLabel(
            350,
            40
        );


    lblPV =
        createLabel(
            50,
            60
        );


    lblBattery =
        createLabel(
            250,
            100
        );


    lblOutside =
        createLabel(
            450,
            60
        );


    lblLiving =
        createLabel(
            350,
            180
        );


    lblNight =
        createLabel(
            500,
            180
        );


    lblCar =
        createLabel(
            250,
            250
        );


    lblInverter =
        createLabel(
            80,
            250
        );

}
void EnergyCard::update()
{


    lv_label_set_text(
        lblPowerFlow,
        "Casa\n350W\nRete 0W"
    );


    lv_label_set_text(
        lblBattery,
        "85%\n+2.3kW\n31°C"
    );


    lv_label_set_text(
        lblOutside,
        "24.1°C\n58%"
    );


    lv_label_set_text(
        lblLiving,
        "22.8°C\n51%"
    );


    lv_label_set_text(
        lblCar,
        "EV\n72%\n7.4kW"
    );

}