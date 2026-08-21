#pragma once

#include <lvgl.h>
#include <string>


// Immagini icona (stile Cute Color / "Dusk" di icons8), convertite con
// il tool https://lvgl.io/tools/imageconverter e compilate come file .c
// nel progetto (vedi src/assets/weather_icons/).
#ifdef __cplusplus
extern "C" {
#endif

LV_IMG_DECLARE(sun_96);
LV_IMG_DECLARE(moon_96);
LV_IMG_DECLARE(sun_cloud_96);
LV_IMG_DECLARE(moon_cloud_96);
LV_IMG_DECLARE(rain_96);
LV_IMG_DECLARE(snow_96);
LV_IMG_DECLARE(wind_96);
LV_IMG_DECLARE(storm_96);

#ifdef __cplusplus
}
#endif


// Card con la sola icona (grande, colorata, statica) della condizione
// meteo corrente, senza testo. Legge weather.openweathermap e sun.sun
// per determinare rispettivamente condizione e ciclo giorno/notte.
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
        Storm,
        Wind,
        Snow
    };

    lv_obj_t* card = nullptr;
    lv_obj_t* iconContainer = nullptr;
    lv_obj_t* iconImage = nullptr;

    Category currentCategory = Category::None;
    bool currentIsNight = false;

    lv_timer_t* updateTimer = nullptr;

    static void timerCallback(lv_timer_t* timer);

    static Category categoryFromState(const std::string& state);

    static bool isNightTime();

    void buildIcon(Category category, bool night);

};