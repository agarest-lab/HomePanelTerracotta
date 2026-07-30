#include "StatusCardsRow.h"
#include "../../services/HomeAssistantRepository.h"
#include <cstdio>
#include <cstring>

lv_obj_t* StatusCardsRow::createCardContainer(
    lv_obj_t* parent,
    int x,
    int y,
    int w,
    int h
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

    return card;
}

lv_obj_t* StatusCardsRow::createValueLabel(
    lv_obj_t* parent,
    int x,
    int y,
    lv_color_t color
)
{
    lv_obj_t* label = lv_label_create(parent);

    lv_obj_set_style_text_color(label, color, 0);
    lv_obj_set_style_text_font(label, &lv_font_montserrat_32, 0);
    lv_obj_align(label, LV_ALIGN_TOP_MID, 0, y);

    return label;
}

void StatusCardsRow::create(
    lv_obj_t* parent,
    int x,
    int y,
    int width,
    int height
)
{
    const int gap = 12;
    const int cardWidth = (width - gap * 3) / 4;

    // --- Card 1: Limite condensa -----------------------------------------

    condensaCard = createCardContainer(parent, x, y, cardWidth, height);

    lv_obj_t* condensaTitle = lv_label_create(condensaCard);
    lv_label_set_text(condensaTitle, "Limite\ncondensa");
    lv_obj_set_style_text_color(condensaTitle, lv_color_hex(0xFF7043), 0);
    lv_obj_set_style_text_font(condensaTitle, &lv_font_montserrat_32, 0);
    lv_obj_align(condensaTitle, LV_ALIGN_TOP_MID, 0, 0);

    condensaValueLabel = createValueLabel(condensaCard, 0, 88, lv_color_hex(0xFF7043));

    // --- Card 2: ACS -------------------------------------------------------

    acsCard = createCardContainer(parent, x + (cardWidth + gap), y, cardWidth, height);

    lv_obj_t* acsTitle = lv_label_create(acsCard);
    lv_label_set_text(acsTitle, "ACS");
    lv_obj_set_style_text_color(acsTitle, lv_color_white(), 0);
    lv_obj_set_style_text_font(acsTitle, &lv_font_montserrat_32, 0);
    lv_obj_align(acsTitle, LV_ALIGN_TOP_MID, 0, 0);

    acsPdcLabel = createValueLabel(acsCard, 0, 44, lv_color_hex(0xFF5252));
    acsCasaLabel = createValueLabel(acsCard, 0, 88, lv_color_hex(0xFFA726));

    // --- Card 3: Impianto ----------------------------------------------

    impiantoCard = createCardContainer(parent, x + 2 * (cardWidth + gap), y, cardWidth, height);

    lv_obj_t* impiantoTitle = lv_label_create(impiantoCard);
    lv_label_set_text(impiantoTitle, "Impianto");
    lv_obj_set_style_text_color(impiantoTitle, lv_color_white(), 0);
    lv_obj_set_style_text_font(impiantoTitle, &lv_font_montserrat_32, 0);
    lv_obj_align(impiantoTitle, LV_ALIGN_TOP_MID, 0, 0);

    mandataLabel = createValueLabel(impiantoCard, 0, 44, lv_color_hex(0xFF8C00));
    ritornoLabel = createValueLabel(impiantoCard, 0, 88, lv_color_hex(0xDAA500));

    // --- Card 4: Allarme -------------------------------------------------

    alarmCard = createCardContainer(parent, x + 3 * (cardWidth + gap), y, cardWidth, height);

    alarmLabel = lv_label_create(alarmCard);
    lv_obj_set_style_text_color(alarmLabel, lv_color_white(), 0);
    lv_obj_set_style_text_font(alarmLabel, &lv_font_montserrat_32, 0);
    lv_label_set_long_mode(alarmLabel, LV_LABEL_LONG_WRAP);
    lv_obj_set_width(alarmLabel, cardWidth - 20);
    lv_obj_set_style_text_align(alarmLabel, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_align(alarmLabel, LV_ALIGN_TOP_MID, 0, 0);

    // Registra tutte le entità presso il repository (nessun I/O di rete
    // dal thread grafico: il polling in background le tiene aggiornate)
    auto& repo = HomeAssistantRepository::instance();

    repo.watch("sensor.ur_limite_anticondensa");
    repo.watch("sensor.acs_pdc");
    repo.watch("sensor.acs_casa");
    repo.watch("sensor.mandata_pdc");
    repo.watch("sensor.ritorno_casa");
    repo.watch("alarm_control_panel.80_000_zone_1_alarm");

    refresh();

    updateTimer = lv_timer_create(timerCallback, 2000, this);
}

void StatusCardsRow::refresh()
{
    auto& repo = HomeAssistantRepository::instance();

    char buf[64];

    // --- Limite condensa ---

    float condensa = repo.getFloatState("sensor.ur_limite_anticondensa");
    std::snprintf(buf, sizeof(buf), "%.1f %", condensa);
    lv_label_set_text(condensaValueLabel, buf);

    // --- ACS ---

    float acsPdc = repo.getFloatState("sensor.acs_pdc");
    float acsCasa = repo.getFloatState("sensor.acs_casa");

    std::snprintf(buf, sizeof(buf), "%.1f °C PDC", acsPdc);
    lv_label_set_text(acsPdcLabel, buf);

    std::snprintf(buf, sizeof(buf), "%.1f °C casa", acsCasa);
    lv_label_set_text(acsCasaLabel, buf);

    if(acsPdc >= 34.0f)
    {
        lv_obj_set_style_bg_color(acsCard, lv_color_hex(0x1B5E20), 0);   // verde scuro
    }
    else
    {
        lv_obj_set_style_bg_color(acsCard, lv_color_hex(0x5E1B1B), 0);   // rosso scuro
    }

    // --- Impianto ---

    float mandata = repo.getFloatState("sensor.mandata_pdc");
    float ritorno = repo.getFloatState("sensor.ritorno_casa");

    std::snprintf(buf, sizeof(buf), "%.1f °C PDC", mandata);
    lv_label_set_text(mandataLabel, buf);

    std::snprintf(buf, sizeof(buf), "%.1f °C casa", ritorno);
    lv_label_set_text(ritornoLabel, buf);


    // --- Allarme ---

    std::string alarmState = repo.getState("alarm_control_panel.80_000_zone_1_alarm");

    if(alarmState == "armed_night")
    {
        lv_label_set_text(alarmLabel, "Allarme:\n\nNOTTE");
        lv_obj_set_style_bg_color(alarmCard, lv_color_hex(0x7A4A00), 0);
    }
    else if(alarmState == "armed_away")
    {
        lv_label_set_text(alarmLabel, "Allarme:\n\nCompleto");
        lv_obj_set_style_bg_color(alarmCard, lv_color_hex(0x5E1B1B), 0);
    }
    else if(alarmState == "disarmed")
    {
        lv_label_set_text(alarmLabel, "Allarme:\n\nDISINSERITO");
        lv_obj_set_style_bg_color(alarmCard, lv_color_hex(0x1B5E20), 0);
    }
    else
    {
        lv_label_set_text(alarmLabel, "Allarme:\nsconosciuto");
        lv_obj_set_style_bg_color(alarmCard, lv_color_hex(0x1A1A1A), 0);
    }
}


void StatusCardsRow::timerCallback(lv_timer_t* timer)
{
    StatusCardsRow* self =
        static_cast<StatusCardsRow*>(
            lv_timer_get_user_data(timer)
        );

    if(self)
    {
        self->refresh();
    }
}
