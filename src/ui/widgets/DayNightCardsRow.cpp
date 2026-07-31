#include "DayNightCardsRow.h"
#include "../../services/HomeAssistantRepository.h"

#include <cstdio>


lv_obj_t* DayNightCardsRow::createCardContainer(
    lv_obj_t* parent,
    int x,
    int y,
    int w,
    int h,
    const char* title
)
{
    lv_obj_t* card = lv_obj_create(parent);

    lv_obj_set_size(card, w, h);
    lv_obj_set_pos(card, x, y);

    lv_obj_set_style_bg_color(card, lv_color_hex(0x1A1A1A), 0);
    lv_obj_set_style_bg_opa(card, LV_OPA_COVER, 0);
    lv_obj_set_style_border_width(card, 0, 0);
    lv_obj_set_style_radius(card, 16, 0);
    lv_obj_set_style_pad_all(card, 10, 0);

    lv_obj_clear_flag(card, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_t* titleLabel = lv_label_create(card);
    lv_label_set_text(titleLabel, title);
    lv_obj_set_style_text_color(titleLabel, lv_color_hex(0xD8D8D8), 0);
    lv_obj_set_style_text_font(titleLabel, &lv_font_montserrat_28, 0);
    lv_obj_align(titleLabel, LV_ALIGN_TOP_MID, 0, 0);

    return card;
}


void DayNightCardsRow::create(
    lv_obj_t* parent,
    int x,
    int y,
    int width,
    int height
)
{
    const int gap = 15;
    const int cardWidth = (width - gap) / 2;

    dayCard = createCardContainer(parent, x, y, cardWidth, height, "Giorno");

    dayTempLabel = lv_label_create(dayCard);
    lv_obj_set_style_text_color(dayTempLabel, lv_color_white(), 0);
    lv_obj_set_style_text_font(dayTempLabel, &lv_font_montserrat_32, 0);
    lv_obj_align(dayTempLabel, LV_ALIGN_TOP_MID, 0, 40);

    dayHumLabel = lv_label_create(dayCard);
    lv_obj_set_style_text_color(dayHumLabel, lv_color_hex(0xA0A0A0), 0);
    lv_obj_set_style_text_font(dayHumLabel, &lv_font_montserrat_28, 0);
    lv_obj_align(dayHumLabel, LV_ALIGN_TOP_MID, 0, 84);


    nightCard = createCardContainer(parent, x + (cardWidth + gap), y, cardWidth, height, "Notte");

    nightTempLabel = lv_label_create(nightCard);
    lv_obj_set_style_text_color(nightTempLabel, lv_color_white(), 0);
    lv_obj_set_style_text_font(nightTempLabel, &lv_font_montserrat_32, 0);
    lv_obj_align(nightTempLabel, LV_ALIGN_TOP_MID, 0, 40);

    nightHumLabel = lv_label_create(nightCard);
    lv_obj_set_style_text_color(nightHumLabel, lv_color_hex(0xA0A0A0), 0);
    lv_obj_set_style_text_font(nightHumLabel, &lv_font_montserrat_28, 0);
    lv_obj_align(nightHumLabel, LV_ALIGN_TOP_MID, 0, 84);


    auto& repo = HomeAssistantRepository::instance();

    repo.watch("sensor.soggiorno_soggiorno_temperatura_soggiorno");
    repo.watch("sensor.soggiorno_soggiorno_umidita_soggiorno");
    repo.watch("sensor.camera_temperatura_camera");
    repo.watch("sensor.camera_umidita_camera");


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
