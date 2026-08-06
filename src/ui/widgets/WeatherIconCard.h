#pragma once

#include <lvgl.h>
#include <string>


// Immagini icona (stile Cute Color / "Dusk" di icons8), convertite con
// il tool https://lvgl.io/tools/imageconverter e compilate come file .c
// nel progetto (vedi src/assets/weather_icons/).
#ifdef __cplusplus
extern "C" {
#endif

LV_IMG_DECLARE(sun_icon);
LV_IMG_DECLARE(clouds_icon);
LV_IMG_DECLARE(rain_icon);
LV_IMG_DECLARE(storm_icon);

#ifdef __cplusplus
}
#endif


// Card con la sola icona (grande, colorata, statica) della condizione
// meteo corrente, senza testo. Legge weather.openweathermap.
class WeatherIconCard
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

    enum class Category
    {
        None,
        Clear,
        Cloudy,
        Rain,
        Storm
    };

    lv_obj_t* card = nullptr;
    lv_obj_t* iconContainer = nullptr;
    lv_obj_t* iconImage = nullptr;

    Category currentCategory = Category::None;

    lv_timer_t* updateTimer = nullptr;

    static void timerCallback(lv_timer_t* timer);

    static Category categoryFromState(const std::string& state);

    void buildIcon(Category category);

};