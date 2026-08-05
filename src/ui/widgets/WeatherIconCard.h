#pragma once

#include <lvgl.h>
#include <string>


// Card con la sola icona (grande, colorata, animata) della condizione
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

    Category currentCategory = Category::None;

    lv_timer_t* updateTimer = nullptr;

    static void timerCallback(lv_timer_t* timer);

    static Category categoryFromState(const std::string& state);

    void buildIcon(Category category);

    void buildClearIcon();
    void buildCloudyIcon();
    void buildRainIcon();
    void buildStormIcon();

    // Helper per disegnare una nuvola semplice (usata da più categorie)
    void addCloudShape(lv_color_t color);

};
