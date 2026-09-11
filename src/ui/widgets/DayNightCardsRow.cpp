#include "DayNightCardsRow.h"
#include "../../services/HomeAssistantRepository.h"

#include <cstdio>


static const int GAP = 16;


lv_obj_t* DayNightCardsRow::createCardContainer(
    lv_obj_t* parent,
    int x,
    int y,
    int w,
    int h,
    const char* title,
    lv_color_t titleColor
)
{
    lv_obj_t* card = lv_obj_create(parent);

    lv_obj_set_size(card, w, h);
    lv_obj_set_pos(card, x, y);

    lv_obj_set_style_bg_color(card, lv_color_hex(0x353535), 0);
    lv_obj_set_style_bg_grad_color(card, lv_color_hex(0x171717), 0);
    lv_obj_set_style_bg_grad_dir(card, LV_GRAD_DIR_RADIAL, 0);
    lv_obj_set_style_bg_opa(card, LV_OPA_COVER, 0);
    lv_obj_set_style_border_width(card, 0, 0);
    lv_obj_set_style_radius(card, 16, 0);
    lv_obj_set_style_pad_all(card, 8, 0);

    lv_obj_clear_flag(card, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_t* titleLabel = lv_label_create(card);
    lv_label_set_text(titleLabel, title);
    lv_obj_set_style_text_color(titleLabel, titleColor, 0);   // <-- usa titleColor
    lv_obj_set_style_text_font(titleLabel, &lv_font_montserrat_32, 0);
    lv_obj_align(titleLabel, LV_ALIGN_TOP_MID, 0, 0);

    return card;
}


void DayNightCardsRow::create(
    lv_obj_t* parent,
    int x,
    int y,
    int cardWidth,
    int bathCardWidth,
    int height
)
{
    dayCard = createCardContainer(
        parent, x, y, cardWidth, height,
        "Giorno", lv_color_hex(0xFFCA28)
    );

    dayTempLabel = lv_label_create(dayCard);
    lv_obj_set_style_text_color(dayTempLabel, lv_color_white(), 0);
    lv_obj_set_style_text_font(dayTempLabel, &lv_font_montserrat_32, 0);
    lv_obj_align(dayTempLabel, LV_ALIGN_TOP_MID, 0, 44);

    dayHumLabel = lv_label_create(dayCard);
    lv_obj_set_style_text_color(dayHumLabel, lv_color_hex(0xA0A0A0), 0);
    lv_obj_set_style_text_font(dayHumLabel, &lv_font_montserrat_32, 0);
    lv_obj_align(dayHumLabel, LV_ALIGN_TOP_MID, 0, 88);


    int nightX = x + cardWidth + GAP;

    nightCard = createCardContainer(
        parent, nightX, y, cardWidth, height,
        "Notte", lv_color_hex(0x4FC3F7)
    );

    nightTempLabel = lv_label_create(nightCard);
    lv_obj_set_style_text_color(nightTempLabel, lv_color_white(), 0);
    lv_obj_set_style_text_font(nightTempLabel, &lv_font_montserrat_32, 0);
    lv_obj_align(nightTempLabel, LV_ALIGN_TOP_MID, 0, 44);

    nightHumLabel = lv_label_create(nightCard);
    lv_obj_set_style_text_color(nightHumLabel, lv_color_hex(0xA0A0A0), 0);
    lv_obj_set_style_text_font(nightHumLabel, &lv_font_montserrat_32, 0);
    lv_obj_align(nightHumLabel, LV_ALIGN_TOP_MID, 0, 88);


    int bathX = nightX + cardWidth + GAP;

    bathCard = createCardContainer(
        parent, bathX, y, bathCardWidth, height,
        "Bagno", lv_color_hex(0x81C784)   // verde per il bagno
    );

    bathTempLabel = lv_label_create(bathCard);
    lv_obj_set_style_text_color(bathTempLabel, lv_color_white(), 0);
    lv_obj_set_style_text_font(bathTempLabel, &lv_font_montserrat_32, 0);
    lv_obj_align(bathTempLabel, LV_ALIGN_TOP_MID, 0, 44);

    bathHumLabel = lv_label_create(bathCard);
    lv_obj_set_style_text_color(bathHumLabel, lv_color_hex(0xA0A0A0), 0);
    lv_obj_set_style_text_font(bathHumLabel, &lv_font_montserrat_32, 0);
    lv_obj_align(bathHumLabel, LV_ALIGN_TOP_MID, 0, 88);


    auto& repo = HomeAssistantRepository::instance();

    repo.watch("sensor.soggiorno_soggiorno_temperatura_soggiorno");
    repo.watch("sensor.soggiorno_soggiorno_umidita_soggiorno");
    repo.watch("sensor.camera_temperatura_camera");
    repo.watch("sensor.camera_umidita_camera");
    repo.watch("sensor.sonoff_temperatura_bagno_notte");
    repo.watch("sensor.sonoff_umidita_bagno_notte");


    refresh();

    updateTimer = lv_timer_create(timerCallback, 2000, this);
}


void DayNightCardsRow::refresh()
{
    auto& repo = HomeAssistantRepository::instance();

    char buf[32];

    float dayTemp = repo.getFloatState("sensor.soggiorno_soggiorno_temperatura_soggiorno");
    float dayHum  = repo.getFloatState("sensor.soggiorno_soggiorno_umidita_soggiorno");

    std::snprintf(buf, sizeof(buf), "%.1f °C", dayTemp);
    lv_label_set_text(dayTempLabel, buf);

    std::snprintf(buf, sizeof(buf), "%.1f %%", dayHum);
    lv_label_set_text(dayHumLabel, buf);


    float nightTemp = repo.getFloatState("sensor.camera_temperatura_camera");
    float nightHum  = repo.getFloatState("sensor.camera_umidita_camera");

    std::snprintf(buf, sizeof(buf), "%.1f °C", nightTemp);
    lv_label_set_text(nightTempLabel, buf);

    std::snprintf(buf, sizeof(buf), "%.1f %%", nightHum);
    lv_label_set_text(nightHumLabel, buf);


    float bathTemp = repo.getFloatState("sensor.sonoff_temperatura_bagno_notte");
    float bathHum  = repo.getFloatState("sensor.sonoff_umidita_bagno_notte");

    std::snprintf(buf, sizeof(buf), "%.1f °C", bathTemp);
    lv_label_set_text(bathTempLabel, buf);

    std::snprintf(buf, sizeof(buf), "%.1f %%", bathHum);
    lv_label_set_text(bathHumLabel, buf);
}

void DayNightCardsRow::timerCallback(lv_timer_t* timer)
{
    DayNightCardsRow* self =
        static_cast<DayNightCardsRow*>(
            lv_timer_get_user_data(timer)
        );

    if(self)
    {
        self->refresh();
    }
}
