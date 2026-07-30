#include "EnergyGaugesPanel.h"
#include "../../services/HomeAssistantRepository.h"

#include <cstdio>
#include <cmath>


EnergyGaugesPanel::Gauge EnergyGaugesPanel::createGauge(
    lv_obj_t* parent,
    int x,
    int y,
    int size,
    const char* title,
    lv_color_t color
)
{
    Gauge gauge;

    gauge.card = lv_obj_create(parent);
    lv_obj_set_size(gauge.card, size, size + 90);
    lv_obj_set_pos(gauge.card, x, y);
    lv_obj_set_style_bg_color(gauge.card, lv_color_hex(0x1A1A1A), 0);
    lv_obj_set_style_bg_opa(gauge.card, LV_OPA_COVER, 0);
    lv_obj_set_style_border_width(gauge.card, 0, 0);
    lv_obj_set_style_radius(gauge.card, 16, 0);
    lv_obj_set_style_pad_all(gauge.card, 8, 0);
    lv_obj_clear_flag(gauge.card, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_t* titleLabel = lv_label_create(gauge.card);
    lv_label_set_text(titleLabel, title);
    lv_obj_set_style_text_color(titleLabel, lv_color_hex(0xD8D8D8), 0);
    lv_obj_set_style_text_font(titleLabel, &lv_font_montserrat_28, 0);
    lv_obj_align(titleLabel, LV_ALIGN_TOP_MID, 0, 0);

    int arcSize = size - 40;

    gauge.arc = lv_arc_create(gauge.card);
    lv_obj_set_size(gauge.arc, arcSize, arcSize);
    lv_arc_set_rotation(gauge.arc, 135);
    lv_arc_set_bg_angles(gauge.arc, 0, 270);
    lv_arc_set_range(gauge.arc, 0, 100);
    lv_arc_set_value(gauge.arc, 0);

    lv_obj_remove_style(gauge.arc, NULL, LV_PART_KNOB);
    lv_obj_clear_flag(gauge.arc, LV_OBJ_FLAG_CLICKABLE);

    lv_obj_set_style_arc_width(gauge.arc, 16, LV_PART_MAIN);
    lv_obj_set_style_arc_color(gauge.arc, lv_color_hex(0x333333), LV_PART_MAIN);

    lv_obj_set_style_arc_width(gauge.arc, 16, LV_PART_INDICATOR);
    lv_obj_set_style_arc_color(gauge.arc, color, LV_PART_INDICATOR);

    lv_obj_align(gauge.arc, LV_ALIGN_TOP_MID, 0, 34);


    gauge.valueLabel = lv_label_create(gauge.arc);
    lv_obj_set_style_text_color(gauge.valueLabel, lv_color_white(), 0);
    lv_obj_set_style_text_font(gauge.valueLabel, &lv_font_montserrat_28, 0);
    lv_obj_center(gauge.valueLabel);

    gauge.subLabel = lv_label_create(gauge.card);
    lv_obj_set_style_text_color(gauge.subLabel, lv_color_hex(0xA0A0A0), 0);
    lv_obj_set_style_text_font(gauge.subLabel, &lv_font_montserrat_28, 0);
    lv_obj_align(gauge.subLabel, LV_ALIGN_BOTTOM_MID, 0, -4);

    return gauge;
}


void EnergyGaugesPanel::setGaugeValue(
    Gauge& gauge,
    int value,
    int maxValue,
    const char* valueText,
    const char* subText,
    lv_color_t arcColor
)
{
    int clamped = value;
    if(clamped < 0) clamped = 0;
    if(clamped > maxValue) clamped = maxValue;

    int percent = maxValue > 0 ? (clamped * 100) / maxValue : 0;

    lv_arc_set_value(gauge.arc, percent);
    lv_obj_set_style_arc_color(gauge.arc, arcColor, LV_PART_INDICATOR);

    lv_label_set_text(gauge.valueLabel, valueText);
    lv_label_set_text(gauge.subLabel, subText);
}


void EnergyGaugesPanel::create(
    lv_obj_t* parent,
    int x,
    int y,
    int width
)
{
    const int gap = 15;
    const int gaugeSize = (width - gap * 3) / 4;

    houseGauge = createGauge(
        parent, x, y, gaugeSize,
        "Casa", lv_color_hex(0x4FC3F7)
    );

    pvGauge = createGauge(
        parent, x + (gaugeSize + gap), y, gaugeSize,
        "FV", lv_color_hex(0xFFCA28)
    );

    batteryGauge = createGauge(
        parent, x + 2 * (gaugeSize + gap), y, gaugeSize,
        "Batteria", lv_color_hex(0x66BB6A)
    );

    gridGauge = createGauge(
        parent, x + 3 * (gaugeSize + gap), y, gaugeSize,
        "Rete", lv_color_hex(0xBA68C8)
    );


    auto& repo = HomeAssistantRepository::instance();

    repo.watch("sensor.potenza_somma_casa");
    repo.watch("sensor.contatore_di_energia_potenza_attiva");
    repo.watch("sensor.inverter_potenza_attiva");
    repo.watch("sensor.inverter_rendimento_giornaliero");
    repo.watch("sensor.batterie_stato_della_capacita");
    repo.watch("sensor.batterie_potenza_di_carica_scarica");


    refresh();

    updateTimer = lv_timer_create(timerCallback, 2000, this);
}


void EnergyGaugesPanel::refresh()
{
    auto& repo = HomeAssistantRepository::instance();

    char valueBuf[32];
    char subBuf[48];


    // --- Casa ---

    float housePower = repo.getFloatState("sensor.potenza_somma_casa");
    std::snprintf(valueBuf, sizeof(valueBuf), "%d W", (int)housePower);
    setGaugeValue(houseGauge, (int)housePower, 6000, valueBuf, "Consumo attuale", lv_color_hex(0x4FC3F7));


    // --- Fotovoltaico ---

    float pvPower = repo.getFloatState("sensor.inverter_potenza_attiva");
    float pvToday = repo.getFloatState("sensor.inverter_rendimento_giornaliero");

    std::snprintf(valueBuf, sizeof(valueBuf), "%d W", (int)pvPower);
    std::snprintf(subBuf, sizeof(subBuf), "Oggi %.1f kWh", pvToday);
    setGaugeValue(pvGauge, (int)pvPower, 6000, valueBuf, subBuf, lv_color_hex(0xFFCA28));


    // --- Batteria ---

    float batterySOC = repo.getFloatState("sensor.batterie_stato_della_capacita");
    float batteryPower = repo.getFloatState("sensor.batterie_potenza_di_carica_scarica");

    std::snprintf(valueBuf, sizeof(valueBuf), "%d%%", (int)batterySOC);

    if(batteryPower > 5)
        std::snprintf(subBuf, sizeof(subBuf), "Carica %d W", (int)batteryPower);
    else if(batteryPower < -5)
        std::snprintf(subBuf, sizeof(subBuf), "Scarica %d W", (int)(-batteryPower));
    else
        std::snprintf(subBuf, sizeof(subBuf), "In equilibrio");

    lv_color_t batteryColor;
    if(batterySOC < 20)      batteryColor = lv_color_hex(0xFF5252);
    else if(batterySOC < 50) batteryColor = lv_color_hex(0xFFCA28);
    else                     batteryColor = lv_color_hex(0x66BB6A);

    setGaugeValue(batteryGauge, (int)batterySOC, 100, valueBuf, subBuf, batteryColor);


    // --- Rete ---

    float gridPower = repo.getFloatState("sensor.contatore_di_energia_potenza_attiva");

    std::snprintf(valueBuf, sizeof(valueBuf), "%d W", (int)std::fabs(gridPower));

    lv_color_t gridColor;

    if(gridPower < -2)
    {
        std::snprintf(subBuf, sizeof(subBuf), "In immissione");
        gridColor = lv_color_hex(0x66BB6A);
    }
    else if(gridPower > 2)
    {
        std::snprintf(subBuf, sizeof(subBuf), "In prelievo");
        gridColor = lv_color_hex(0xFF7043);
    }
    else
    {
        std::snprintf(subBuf, sizeof(subBuf), "In equilibrio");
        gridColor = lv_color_hex(0xBA68C8);
    }

    setGaugeValue(gridGauge, (int)std::fabs(gridPower), 6000, valueBuf, subBuf, gridColor);
}


void EnergyGaugesPanel::timerCallback(lv_timer_t* timer)
{
    EnergyGaugesPanel* self =
        static_cast<EnergyGaugesPanel*>(
            lv_timer_get_user_data(timer)
        );

    if(self)
    {
        self->refresh();
    }
}
