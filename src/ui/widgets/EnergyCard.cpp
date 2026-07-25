#include "EnergyCard.h"
#include "../../assets/energy_bg.h"
#include <iostream>
#define ENERGY_CARD_WIDTH  595
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

    lv_obj_add_flag(container, LV_OBJ_FLAG_OVERFLOW_VISIBLE);

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


    // SFONDO PRIMA
    image = lv_img_create(container);

    lv_img_set_src(
        image,
        &energy_bg
    );

    lv_img_set_zoom(image, 341);   // 256 = 100%, 341 ≈ 133% (fattore 4/3)

    lv_obj_set_pos(
        image,
        -35 ,
        -35
    );


    // manda l'immagine dietro
    lv_obj_move_background(image);

    lblPowerFlow =
        createLabel(
            420,
            25
        );


    lblPV =
        createLabel(
            10,
            80
        );


    lblBattery =
        createLabel(
            140,
            280
        );


    lblOutside =
        createLabel(
            120,
            20
        );


    lblLiving =
        createLabel(
            320,
            90
        );


    lblNight =
        createLabel(
            320,
            40
        );


    lblCar =
        createLabel(
            470,
            270
        );


    lblInverter =
        createLabel(
            320,
            280
        );

}
void EnergyCard::update(
    const EnergyData& data
)

{
    std::cout 
    << "Energy update: "
    << data.housePower
    << " W PV "
    << data.pvPower
    << std::endl;

    std::cout 
        << "ENERGY CARD UPDATE "
        << data.livingTemp
        << " "
        << data.livingHum
        << std::endl;

    lv_label_set_text_fmt(
        lblPowerFlow,
        "Casa\n%dW\nRete %dW",
        (int)data.housePower,
        (int)data.gridPower
    );


    lv_label_set_text_fmt(
        lblBattery,
        "%d%%\n%dW\n%d°C",
        (int)data.batterySOC,
        (int)data.batteryPower,
        (int)data.batteryTemp
    );


    lv_label_set_text_fmt(
        lblOutside,
        "%d°C\n%d%%",
        (int)data.outsideTemp,
        (int)data.outsideHum
    );


    lv_label_set_text_fmt(
        lblLiving,
        "%d°C\n%d%%",
        (int)data.livingTemp,
        (int)data.livingHum
    );


    lv_label_set_text_fmt(
        lblCar,
        "EV\n%d%%\n%.1fkW",
        (int)data.carSOC,
        data.carPower / 1000.0f
    );

    lv_label_set_text_fmt(
        lblPV,
        "FV\n%dW\nOggi %dkWh",
        (int)data.pvPower,
        (int)data.pvTodayKwh
    );


    lv_label_set_text_fmt(
        lblNight,
        "%d°C\n%d%%",
        (int)data.nightTemp,
        (int)data.nightHum
    );


    lv_label_set_text_fmt(
        lblInverter,
        "INV\n%dW\n%d°C",
        (int)data.inverterPower,
        (int)data.inverterTemp
    );
}