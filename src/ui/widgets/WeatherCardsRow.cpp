#include "WeatherCardsRow.h"
#include "../../services/HomeAssistantRepository.h"

#include <cstdio>
#include <cstring>
#include <algorithm>


lv_obj_t* WeatherCardsRow::createCardContainer(
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


lv_obj_t* WeatherCardsRow::createValueLabel(
    lv_obj_t* parent,
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


std::string WeatherCardsRow::extractTime(const std::string& isoDateTime)
{
    size_t tPos = isoDateTime.find('T');

    if(tPos == std::string::npos || tPos + 6 > isoDateTime.size())
        return "";

    return isoDateTime.substr(tPos + 1, 5);   // "HH:MM"
}


void WeatherCardsRow::create(
    lv_obj_t* parent,
    int x,
    int y,
    int width,
    int height
)
{
    const int gap = 12;
    const int cardWidth = (width - gap * 3) / 4;


    // --- Card 1: Meteo -----------------------------------------------------

    meteoCard = createCardContainer(parent, x, y, cardWidth, height);

    lv_obj_t* meteoTitle = lv_label_create(meteoCard);
    lv_label_set_text(meteoTitle, "Meteo");
    lv_obj_set_style_text_color(meteoTitle, lv_color_hex(0xD8D8D8), 0);
    lv_obj_set_style_text_font(meteoTitle, &lv_font_montserrat_32, 0);
    lv_obj_align(meteoTitle, LV_ALIGN_TOP_MID, 0, 0);

    meteoTempLabel = createValueLabel(meteoCard, 44, lv_color_white());
    meteoCondLabel = createValueLabel(meteoCard, 88, lv_color_hex(0xC0C0C0));


    // --- Card 2: Allerta meteo --------------------------------------------

    alertCard = createCardContainer(parent, x + (cardWidth + gap), y, cardWidth, height);

    alertTitleLabel = lv_label_create(alertCard);
    lv_obj_set_style_text_color(alertTitleLabel, lv_color_white(), 0);
    lv_obj_set_style_text_font(alertTitleLabel, &lv_font_montserrat_32, 0);
    lv_obj_set_width(alertTitleLabel, cardWidth - 20);
    lv_label_set_long_mode(alertTitleLabel, LV_LABEL_LONG_WRAP);
    lv_obj_set_style_text_align(alertTitleLabel, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_align(alertTitleLabel, LV_ALIGN_TOP_MID, 0, 0);

    alertStatusLabel = createValueLabel(alertCard, 88, lv_color_hex(0xE0E0E0));


    // --- Card 3: Qualità aria ----------------------------------------------

    aqiCard = createCardContainer(parent, x + 2 * (cardWidth + gap), y, cardWidth, height);

    lv_obj_t* aqiTitle = lv_label_create(aqiCard);
    lv_label_set_text(aqiTitle, "Air Quality");
    lv_obj_set_style_text_color(aqiTitle, lv_color_hex(0xD8D8D8), 0);
    lv_obj_set_style_text_font(aqiTitle, &lv_font_montserrat_32, 0);
    lv_obj_align(aqiTitle, LV_ALIGN_TOP_MID, 0, 0);

    aqiValueLabel = createValueLabel(aqiCard, 44, lv_color_white());
    aqiPollutantLabel = createValueLabel(aqiCard, 88, lv_color_hex(0xC0C0C0));


    // --- Card 4: Stazione ---------------------------------------------------

    stationCard = createCardContainer(parent, x + 3 * (cardWidth + gap), y, cardWidth, height);

    lv_obj_t* stationTitle = lv_label_create(stationCard);
    lv_label_set_text(stationTitle, "Stazione");
    lv_obj_set_style_text_color(stationTitle, lv_color_hex(0xD8D8D8), 0);
    lv_obj_set_style_text_font(stationTitle, &lv_font_montserrat_32, 0);
    lv_obj_align(stationTitle, LV_ALIGN_TOP_MID, 0, 0);

    stationNameLabel = createValueLabel(stationCard, 44, lv_color_white());
    stationPm25Label = createValueLabel(stationCard, 88, lv_color_hex(0xC0C0C0));


    // Registra le entità presso il repository
    auto& repo = HomeAssistantRepository::instance();

    repo.watch("sensor.openweathermap_temperature");
    repo.watch("sensor.openweathermap_weather");

    repo.watch("binary_sensor.meteoalarm_friuli_venezia_giulia");

    repo.watch("sensor.aqi_peggiore");
    repo.watch("sensor.inquinante_dominante_peggiore");

    repo.watch("sensor.stazione_aqi_peggiore");
    repo.watch("sensor.via_meduna_pm2_5");
    repo.watch("sensor.via_dei_cipressi_pm2_5");


    refresh();

    updateTimer = lv_timer_create(timerCallback, 2000, this);
}


void WeatherCardsRow::refresh()
{
    auto& repo = HomeAssistantRepository::instance();

    char buf[64];


    // --- Meteo ---------------------------------------------------------

    float temp = repo.getFloatState("sensor.openweathermap_temperature");
    std::string weather = repo.getState("sensor.openweathermap_weather");

    std::snprintf(buf, sizeof(buf), "%.1f °C", temp);
    lv_label_set_text(meteoTempLabel, buf);
    lv_label_set_text(meteoCondLabel, weather.empty() ? "--" : weather.c_str());

    lv_color_t meteoColor;

    if(temp < 0)        meteoColor = lv_color_hex(0x0078FF);
    else if(temp < 5)   meteoColor = lv_color_hex(0x00B4FF);
    else if(temp < 15)  meteoColor = lv_color_hex(0x00FFC8);
    else if(temp < 25)  meteoColor = lv_color_hex(0xFFDC00);
    else if(temp < 32)  meteoColor = lv_color_hex(0xFF8C00);
    else                meteoColor = lv_color_hex(0xFF3C00);

    lv_obj_set_style_bg_color(meteoCard, meteoColor, 0);
    lv_obj_set_style_bg_opa(meteoCard, LV_OPA_30, 0);


    // --- Allerta meteo ---------------------------------------------------

    std::string alarmState = repo.getState("binary_sensor.meteoalarm_friuli_venezia_giulia");

    std::string awarenessRaw = repo.getAttribute("binary_sensor.meteoalarm_friuli_venezia_giulia", "awareness_level");
    std::string eventRaw = repo.getAttribute("binary_sensor.meteoalarm_friuli_venezia_giulia", "event");
    std::string expiresRaw = repo.getAttribute("binary_sensor.meteoalarm_friuli_venezia_giulia", "expires");

    // awareness_level tipicamente "2; qualcosa" -> prendiamo solo la prima parte
    std::string level = awarenessRaw;
    size_t semiPos = awarenessRaw.find(';');
    if(semiPos != std::string::npos)
        level = awarenessRaw.substr(0, semiPos);

    // trim spazi
    level.erase(std::remove(level.begin(), level.end(), ' '), level.end());


    std::string eventLower = eventRaw;
    std::transform(eventLower.begin(), eventLower.end(), eventLower.begin(), ::tolower);

    std::string eventLabel = "Allerta";
    if(eventLower.find("rain") != std::string::npos)        eventLabel = "Pioggia";
    else if(eventLower.find("storm") != std::string::npos)  eventLabel = "Temporali";
    else if(eventLower.find("wind") != std::string::npos)   eventLabel = "Vento";
    else if(eventLower.find("snow") != std::string::npos)   eventLabel = "Neve";
    else if(eventLower.find("temperature") != std::string::npos) eventLabel = "Temperatura";
    else if(!eventRaw.empty())                               eventLabel = eventRaw;

    lv_label_set_text(alertTitleLabel, eventLabel.c_str());

    if(expiresRaw.empty())
    {
        lv_label_set_text(alertStatusLabel, "Non presente");
    }
    else
    {
        std::string time = extractTime(expiresRaw);
        std::snprintf(buf, sizeof(buf), "Fino alle %s", time.empty() ? "--:--" : time.c_str());
        lv_label_set_text(alertStatusLabel, buf);
    }

    lv_color_t alertColor;

    if(alarmState == "off" || level == "0" || level.empty())
        alertColor = lv_color_hex(0x008000);
    else if(level == "2")
        alertColor = lv_color_hex(0xF1C40F);
    else if(level == "3")
        alertColor = lv_color_hex(0xFFA500);
    else if(level == "4")
        alertColor = lv_color_hex(0xFF0000);
    else
        alertColor = lv_color_hex(0x6E2C00);

    lv_obj_set_style_bg_color(alertCard, alertColor, 0);
    lv_obj_set_style_bg_opa(alertCard, LV_OPA_40, 0);


    // --- Qualità aria ------------------------------------------------------

    float aqi = repo.getFloatState("sensor.aqi_peggiore");
    std::string pollutant = repo.getState("sensor.inquinante_dominante_peggiore");

    std::snprintf(buf, sizeof(buf), "AQI %d", (int)aqi);
    lv_label_set_text(aqiValueLabel, buf);

    std::string pollutantLower = pollutant;
    std::transform(pollutantLower.begin(), pollutantLower.end(), pollutantLower.begin(), ::tolower);
    pollutantLower.erase(std::remove(pollutantLower.begin(), pollutantLower.end(), '.'), pollutantLower.end());

    std::string pollutantLabel;

    if(pollutant.empty() || pollutant == "unknown" || pollutant == "unavailable")
        pollutantLabel = "Inq --";
    else if(pollutantLower == "pm25")
        pollutantLabel = "Inq PM2.5";
    else if(pollutantLower == "pm10")
        pollutantLabel = "Inq PM10";
    else if(pollutantLower == "pm1")
        pollutantLabel = "Inq PM1";
    else if(pollutantLower == "no2")
        pollutantLabel = "Inq NO2";
    else if(pollutantLower == "o3")
        pollutantLabel = "Inq O3";
    else if(pollutantLower == "so2")
        pollutantLabel = "Inq SO2";
    else if(pollutantLower == "co")
        pollutantLabel = "Inq CO";
    else if(pollutantLower == "nh3")
        pollutantLabel = "Inq NH3";
    else
        pollutantLabel = "Inq " + pollutant;

    lv_label_set_text(aqiPollutantLabel, pollutantLabel.c_str());

    lv_color_t aqiColor;

    if(aqi <= 50)        aqiColor = lv_color_hex(0x008000);
    else if(aqi <= 100)  aqiColor = lv_color_hex(0xF1C40F);
    else if(aqi <= 150)  aqiColor = lv_color_hex(0xFFA500);
    else if(aqi <= 200)  aqiColor = lv_color_hex(0xFF0000);
    else if(aqi <= 300)  aqiColor = lv_color_hex(0x9B59B6);
    else                 aqiColor = lv_color_hex(0x6E2C00);

    lv_obj_set_style_bg_color(aqiCard, aqiColor, 0);
    lv_obj_set_style_bg_opa(aqiCard, LV_OPA_40, 0);


    // --- Stazione ---------------------------------------------------------

    std::string stationName = repo.getState("sensor.stazione_aqi_peggiore");

    lv_label_set_text(stationNameLabel, stationName.empty() ? "--" : stationName.c_str());

    float pm25 = 0.0f;
    bool pm25Valid = false;

    if(stationName == "Via Meduna")
    {
        pm25 = repo.getFloatState("sensor.via_meduna_pm2_5");
        pm25Valid = true;
    }
    else if(stationName == "Via dei Cipressi")
    {
        pm25 = repo.getFloatState("sensor.via_dei_cipressi_pm2_5");
        pm25Valid = true;
    }

    if(pm25Valid)
    {
        std::snprintf(buf, sizeof(buf), "%.1f ug/mc", pm25);
    }
    else
    {
        std::snprintf(buf, sizeof(buf), "-- ug/mc");
    }

    lv_label_set_text(stationPm25Label, buf);

    // Stessa scala colori dell'AQI, riusando il valore della stazione
    // peggiore (sensor.aqi_peggiore) come nel dashboard originale
    lv_obj_set_style_bg_color(stationCard, aqiColor, 0);
    lv_obj_set_style_bg_opa(stationCard, LV_OPA_20, 0);
}


void WeatherCardsRow::timerCallback(lv_timer_t* timer)
{
    WeatherCardsRow* self =
        static_cast<WeatherCardsRow*>(
            lv_timer_get_user_data(timer)
        );

    if(self)
    {
        self->refresh();
    }
}
