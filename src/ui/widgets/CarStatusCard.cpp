#include "CarStatusCard.h"
#include "../../services/HomeAssistantRepository.h"

#include <cstdio>


std::string CarStatusCard::formatDateTime(const std::string& raw)
{
    size_t tPos = raw.find('T');

    if(tPos == std::string::npos || raw.size() < 10)
        return raw;

    std::string datePart = raw.substr(0, 10);   // YYYY-MM-DD
    std::string dd = datePart.substr(8, 2);
    std::string mm = datePart.substr(5, 2);

    std::string timePart;
    if(raw.size() >= tPos + 6)
        timePart = raw.substr(tPos + 1, 5);      // HH:MM

    if(timePart.empty())
        return dd + "/" + mm;

    return dd + "/" + mm + "  " + timePart;
}


void CarStatusCard::create(
    lv_obj_t* parent,
    int x,
    int y,
    int width,
    int height
)
{
    card = lv_obj_create(parent);

    lv_obj_set_size(card, width, height);
    lv_obj_set_pos(card, x, y);

    // lv_obj_set_style_bg_color(card, lv_color_hex(0x1A1A1A), 0);  // Colore fisso originale
    lv_obj_set_style_bg_color(card, lv_color_hex(0x353535), 0);        // Colore iniziale
    lv_obj_set_style_bg_grad_color(card, lv_color_hex(0x171717), 0);  // Colore finale
    lv_obj_set_style_bg_grad_dir(card, LV_GRAD_DIR_RADIAL, 0);
    lv_obj_set_style_bg_opa(card, LV_OPA_COVER, 0);
    lv_obj_set_style_border_width(card, 0, 0);
    lv_obj_set_style_radius(card, 16, 0);
    lv_obj_set_style_pad_all(card, 10, 0);

    lv_obj_clear_flag(card, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_t* title = lv_label_create(card);
    lv_label_set_text(title, "Auto");
    lv_obj_set_style_text_color(title, lv_color_hex(0xD8D8D8), 0);
    lv_obj_set_style_text_font(title, &lv_font_montserrat_28, 0);
    lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 0);

    socLabel = lv_label_create(card);
    lv_obj_set_style_text_color(socLabel, lv_color_white(), 0);
    lv_obj_set_style_text_font(socLabel, &lv_font_montserrat_32, 0);
    lv_obj_align(socLabel, LV_ALIGN_TOP_MID, 0, 38);

    energyLabel = lv_label_create(card);
    lv_obj_set_style_text_color(energyLabel, lv_color_hex(0xA0A0A0), 0);
    lv_obj_set_style_text_font(energyLabel, &lv_font_montserrat_20, 0);
    lv_obj_align(energyLabel, LV_ALIGN_TOP_MID, 0, 82);

    dateLabel = lv_label_create(card);
    lv_obj_set_style_text_color(dateLabel, lv_color_hex(0xA0A0A0), 0);
    lv_obj_set_style_text_font(dateLabel, &lv_font_montserrat_20, 0);
    lv_obj_align(dateLabel, LV_ALIGN_TOP_MID, 0, 112);


    auto& repo = HomeAssistantRepository::instance();

    repo.watch("sensor.saic_mg_mg4_soc");
    repo.watch("sensor.ultima_sessione_energia");
    repo.watch("sensor.ultima_sessione_timestamp");


    refresh();

    updateTimer = lv_timer_create(timerCallback, 2000, this);
}


void CarStatusCard::refresh()
{
    auto& repo = HomeAssistantRepository::instance();

    char buf[48];

    float soc = repo.getFloatState("sensor.saic_mg_mg4_soc");
    std::snprintf(buf, sizeof(buf), "SOC %d%%", (int)soc);
    lv_label_set_text(socLabel, buf);

    float lastEnergy = repo.getFloatState("sensor.ultima_sessione_energia");
    std::snprintf(buf, sizeof(buf), "Ultima ricarica: %.1f kWh", lastEnergy);
    lv_label_set_text(energyLabel, buf);

    std::string lastTimestamp = repo.getState("sensor.ultima_sessione_timestamp");
    std::string formatted = formatDateTime(lastTimestamp);
    std::string dateText = "Il " + formatted;
    lv_label_set_text(dateLabel, dateText.c_str());
}


void CarStatusCard::timerCallback(lv_timer_t* timer)
{
    CarStatusCard* self =
        static_cast<CarStatusCard*>(
            lv_timer_get_user_data(timer)
        );

    if(self)
    {
        self->refresh();
    }
}
