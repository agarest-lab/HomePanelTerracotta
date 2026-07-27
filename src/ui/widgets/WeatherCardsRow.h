#pragma once

#include <lvgl.h>
#include <string>


// Riga di 4 card affiancate che mostrano:
// - Meteo (temperatura + condizione, colore in base alla temperatura)
// - Allerta meteo (colore in base al livello di allerta)
// - Qualità dell'aria (AQI + inquinante dominante, colore in base all'AQI)
// - Stazione di rilevamento più vicina/peggiore (nome + PM2.5)
class WeatherCardsRow
{

public:

    void create(
        lv_obj_t* parent,
        int x,
        int y,
        int width,
        int height
    );

    void refresh();


private:

    lv_obj_t* meteoCard = nullptr;
    lv_obj_t* meteoTempLabel = nullptr;
    lv_obj_t* meteoCondLabel = nullptr;

    lv_obj_t* alertCard = nullptr;
    lv_obj_t* alertTitleLabel = nullptr;
    lv_obj_t* alertStatusLabel = nullptr;

    lv_obj_t* aqiCard = nullptr;
    lv_obj_t* aqiValueLabel = nullptr;
    lv_obj_t* aqiPollutantLabel = nullptr;

    lv_obj_t* stationCard = nullptr;
    lv_obj_t* stationNameLabel = nullptr;
    lv_obj_t* stationPm25Label = nullptr;

    lv_timer_t* updateTimer = nullptr;

    static void timerCallback(lv_timer_t* timer);

    lv_obj_t* createCardContainer(
        lv_obj_t* parent,
        int x,
        int y,
        int w,
        int h
    );

    lv_obj_t* createValueLabel(
        lv_obj_t* parent,
        int y,
        lv_color_t color
    );

    // Estrae "HH:MM" da una stringa ISO tipo "2026-07-27T18:30:00+00:00"
    // Nota: mostra l'orario così come riportato da Home Assistant (di solito
    // UTC), senza conversione al fuso orario locale del dispositivo.
    static std::string extractTime(const std::string& isoDateTime);

};
