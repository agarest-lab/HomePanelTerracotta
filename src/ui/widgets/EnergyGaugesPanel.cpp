#include "EnergyGaugesPanel.h"
#include "../../services/HomeAssistantRepository.h"

#include <cstdio>
#include <cmath>


static const lv_color_t COLOR_NEUTRAL = lv_color_hex(0xA0A0A0);
static const lv_color_t COLOR_RED     = lv_color_hex(0xFF5252);
static const lv_color_t COLOR_GREEN   = lv_color_hex(0x66BB6A);


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

    // +156: spazio per titolo, arco, riga extra (font più grande, fino a
    // 2 righe) e subLabel
    gauge.card = lv_obj_create(parent);
    lv_obj_set_size(gauge.card, size, size + 115);
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

    // Riga extra: usata da Rete (energia oggi), Batteria (temp BMS) e
    // FV (temp inverter). Font ingrandito rispetto a prima, con wrap
    // nel caso della Rete (2 righe).
    gauge.extraLabel = lv_label_create(gauge.card);
    lv_label_set_text(gauge.extraLabel, "");
    lv_obj_set_style_text_color(gauge.extraLabel, lv_color_hex(0xC0C0C0), 0);
    lv_obj_set_style_text_font(gauge.extraLabel, &lv_font_montserrat_28, 0);
    lv_obj_set_width(gauge.extraLabel, size - 16);
    lv_label_set_long_mode(gauge.extraLabel, LV_LABEL_LONG_WRAP);
    lv_obj_set_style_text_align(gauge.extraLabel, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_align(gauge.extraLabel, LV_ALIGN_BOTTOM_MID, 0, -46);

    gauge.subLabel = lv_label_create(gauge.card);
    lv_obj_set_style_text_color(gauge.subLabel, COLOR_NEUTRAL, 0);
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
    lv_color_t arcColor,
    lv_color_t subTextColor
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
    lv_obj_set_style_text_color(gauge.subLabel, subTextColor, 0);
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


    // Gauge interno più piccolo dentro la card "Casa", per
    // sensor.1p7k_new_potenza (attenzione: quel sensore riporta
    // il valore già in kW, non W)
    {
        int outerArcSize = gaugeSize - 40;
        int innerArcSize = (int)(outerArcSize * 0.55f);

        houseGauge.innerArc = lv_arc_create(houseGauge.card);
        lv_obj_set_size(houseGauge.innerArc, innerArcSize, innerArcSize);
        lv_arc_set_rotation(houseGauge.innerArc, 135);
        lv_arc_set_bg_angles(houseGauge.innerArc, 0, 270);
        lv_arc_set_range(houseGauge.innerArc, 0, 100);
        lv_arc_set_value(houseGauge.innerArc, 0);

        lv_obj_remove_style(houseGauge.innerArc, NULL, LV_PART_KNOB);
        lv_obj_clear_flag(houseGauge.innerArc, LV_OBJ_FLAG_CLICKABLE);

        lv_obj_set_style_arc_width(houseGauge.innerArc, 10, LV_PART_MAIN);
        lv_obj_set_style_arc_color(houseGauge.innerArc, lv_color_hex(0x2A2A2A), LV_PART_MAIN);

        lv_obj_set_style_arc_width(houseGauge.innerArc, 10, LV_PART_INDICATOR);
        lv_obj_set_style_arc_color(houseGauge.innerArc, lv_color_hex(0xE57373), LV_PART_INDICATOR);

        lv_obj_center(houseGauge.innerArc);

        houseGauge.innerValueLabel = lv_label_create(houseGauge.innerArc);
        lv_obj_set_style_text_color(houseGauge.innerValueLabel, lv_color_hex(0xE57373), 0);
        lv_obj_set_style_text_font(houseGauge.innerValueLabel, &lv_font_montserrat_14, 0);
        lv_obj_center(houseGauge.innerValueLabel);

        lv_obj_align(houseGauge.valueLabel, LV_ALIGN_CENTER, 0, -((outerArcSize - innerArcSize) / 4) - 20);
    }


    auto& repo = HomeAssistantRepository::instance();

    repo.watch("sensor.potenza_somma_casa");
    repo.watch("sensor.contatore_di_energia_potenza_attiva");
    repo.watch("sensor.inverter_potenza_attiva");
    repo.watch("sensor.inverter_rendimento_giornaliero");
    repo.watch("sensor.batterie_stato_della_capacita");
    repo.watch("sensor.batterie_potenza_di_carica_scarica");
    repo.watch("sensor.en_importata_sun_oggi");
    repo.watch("sensor.en_immessa_sun_oggi");
    repo.watch("sensor.1p7k_new_potenza");
    repo.watch("sensor.batteria_1_temperatura_bms");
    repo.watch("sensor.inverter_internal_temperature");


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
    setGaugeValue(
        houseGauge, (int)housePower, 6000,
        valueBuf, "Consumo attuale",
        lv_color_hex(0x4FC3F7), COLOR_NEUTRAL
    );

    float extraPowerKw = repo.getFloatState("sensor.1p7k_new_potenza");
    int extraPowerW = (int)(extraPowerKw * 1000.0f);

    int extraClamped = extraPowerW;
    if(extraClamped < 0) extraClamped = 0;
    if(extraClamped > 4000) extraClamped = 4000;

    lv_arc_set_value(houseGauge.innerArc, (extraClamped * 100) / 4000);

    char innerBuf[16];
    std::snprintf(innerBuf, sizeof(innerBuf), "%.1f kW", extraPowerKw);
    lv_label_set_text(houseGauge.innerValueLabel, innerBuf);


    // --- Fotovoltaico ---

    float pvPower = repo.getFloatState("sensor.inverter_potenza_attiva");
    float pvToday = repo.getFloatState("sensor.inverter_rendimento_giornaliero");
    float inverterTemp = repo.getFloatState("sensor.inverter_internal_temperature");

    std::snprintf(valueBuf, sizeof(valueBuf), "%d W", (int)pvPower);
    std::snprintf(subBuf, sizeof(subBuf), "Oggi %.1f kWh", pvToday);

    char inverterTempBuf[32];
    std::snprintf(inverterTempBuf, sizeof(inverterTempBuf), "INV %.0f°C", inverterTemp);
    lv_label_set_text(pvGauge.extraLabel, inverterTempBuf);

    setGaugeValue(
        pvGauge, (int)pvPower, 6000,
        valueBuf, subBuf,
        lv_color_hex(0xFFCA28), COLOR_NEUTRAL
    );


    // --- Batteria ---

    float batterySOC = repo.getFloatState("sensor.batterie_stato_della_capacita");
    float batteryPower = repo.getFloatState("sensor.batterie_potenza_di_carica_scarica");
    float bmsTemp = repo.getFloatState("sensor.batteria_1_temperatura_bms");

    std::snprintf(valueBuf, sizeof(valueBuf), "%d%%", (int)batterySOC);

    char bmsTempBuf[32];
    std::snprintf(bmsTempBuf, sizeof(bmsTempBuf), "BMS %.0f°C", bmsTemp);
    lv_label_set_text(batteryGauge.extraLabel, bmsTempBuf);

    lv_color_t batterySubColor;

    if(batteryPower > 5)
    {
        std::snprintf(subBuf, sizeof(subBuf), "Carica %d W", (int)batteryPower);
        batterySubColor = COLOR_GREEN;
    }
    else if(batteryPower < -5)
    {
        std::snprintf(subBuf, sizeof(subBuf), "Scarica %d W", (int)(-batteryPower));
        batterySubColor = COLOR_RED;
    }
    else
    {
        std::snprintf(subBuf, sizeof(subBuf), "In equilibrio");
        batterySubColor = COLOR_NEUTRAL;
    }

    lv_color_t batteryArcColor;
    if(batterySOC < 20)      batteryArcColor = COLOR_RED;
    else if(batterySOC < 50) batteryArcColor = lv_color_hex(0xFFCA28);
    else                     batteryArcColor = COLOR_GREEN;

    setGaugeValue(
        batteryGauge, (int)batterySOC, 100,
        valueBuf, subBuf,
        batteryArcColor, batterySubColor
    );


    // --- Rete ---

    float gridPower = repo.getFloatState("sensor.contatore_di_energia_potenza_attiva");
    float importedToday = repo.getFloatState("sensor.en_importata_sun_oggi");
    float exportedToday = repo.getFloatState("sensor.en_immessa_sun_oggi");

    std::snprintf(valueBuf, sizeof(valueBuf), "%d W", (int)std::fabs(gridPower));

    char energyTodayBuf[48];
    std::snprintf(
        energyTodayBuf, sizeof(energyTodayBuf),
        "Prel %.1f kWh\nImm %.1f kWh",
        importedToday, exportedToday
    );
    lv_label_set_text(gridGauge.extraLabel, energyTodayBuf);

    lv_color_t gridArcColor;
    lv_color_t gridSubColor;

    if(gridPower > 10)
    {
        std::snprintf(subBuf, sizeof(subBuf), "In immissione");
        gridArcColor = COLOR_GREEN;
        gridSubColor = COLOR_GREEN;
    }
    else if(gridPower < -10)
    {
        std::snprintf(subBuf, sizeof(subBuf), "In prelievo");
        gridArcColor = lv_color_hex(0xFF7043);
        gridSubColor = COLOR_RED;
    }
    else
    {
        std::snprintf(subBuf, sizeof(subBuf), "In equilibrio");
        gridArcColor = lv_color_hex(0xBA68C8);
        gridSubColor = COLOR_NEUTRAL;
    }

    setGaugeValue(
        gridGauge, (int)std::fabs(gridPower), 6000,
        valueBuf, subBuf,
        gridArcColor, gridSubColor
    );
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
