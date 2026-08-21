#include "WeatherIconCard.h"
#include "../../services/HomeAssistantRepository.h"


WeatherIconCard::Category WeatherIconCard::categoryFromState(const std::string& state)
{
    if(state == "sunny" || state == "clear-night")
        return Category::Clear;

    if(state == "cloudy" || state == "partlycloudy" || state == "fog")
        return Category::Cloudy;

    if(state == "windy" || state == "windy-variant")
        return Category::Wind;

    if(state == "rainy" || state == "pouring" || state == "hail" || state == "snowy-rainy")
        return Category::Rain;

    if(state == "snowy")
        return Category::Snow;

    if(state == "lightning" || state == "lightning-rainy" || state == "exceptional")
        return Category::Storm;

    return Category::Cloudy;
}


bool WeatherIconCard::isNightTime()
{
    auto& repo = HomeAssistantRepository::instance();

    // sun.sun ha stato "below_horizon" di notte, "above_horizon" di giorno
    std::string sunState = repo.getState("sun.sun");
    if(sunState == "below_horizon")
        return true;

    // fallback: se lo stato meteo stesso indica la notte
    std::string weatherState = repo.getState("weather.openweathermap");
    if(weatherState == "clear-night")
        return true;

    return false;
}


void WeatherIconCard::create(
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

    lv_obj_set_style_bg_color(card, lv_color_hex(0x353535), 0);
    lv_obj_set_style_bg_grad_color(card, lv_color_hex(0x171717), 0);
    lv_obj_set_style_bg_grad_dir(card, LV_GRAD_DIR_RADIAL, 0);
    lv_obj_set_style_bg_opa(card, LV_OPA_COVER, 0);
    lv_obj_set_style_border_width(card, 0, 0);
    lv_obj_set_style_radius(card, 16, 0);
    lv_obj_set_style_pad_all(card, 0, 0);

    lv_obj_clear_flag(card, LV_OBJ_FLAG_SCROLLABLE);

    // iconContainer resta solo per centrare/dimensionare l'immagine
    iconContainer = lv_obj_create(card);

    int iconSize = (width < height ? width : height) - 30;
    if(iconSize < 40) iconSize = 40;

    lv_obj_set_size(iconContainer, iconSize, iconSize);
    lv_obj_center(iconContainer);
    lv_obj_set_style_bg_opa(iconContainer, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(iconContainer, 0, 0);
    lv_obj_set_style_pad_all(iconContainer, 0, 0);
    lv_obj_clear_flag(iconContainer, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_clear_flag(iconContainer, LV_OBJ_FLAG_CLICKABLE);

    iconImage = lv_img_create(iconContainer);
    lv_obj_center(iconImage);

    auto& repo = HomeAssistantRepository::instance();
    repo.watch("weather.openweathermap");
    repo.watch("sun.sun");

    refresh();

    updateTimer = lv_timer_create(timerCallback, 5000, this);
}


void WeatherIconCard::buildIcon(Category category, bool night)
{
    const lv_img_dsc_t* icon = &sun_96;

    switch(category)
    {
        case Category::Clear:
            icon = night ? &moon_96 : &sun_96;
            break;

        case Category::Cloudy:
            icon = night ? &moon_cloud_96 : &sun_cloud_96;
            break;

        case Category::Wind:
            icon = &wind_96;
            break;

        case Category::Rain:
            icon = &rain_96;
            break;

        case Category::Snow:
            icon = &snow_96;
            break;

        case Category::Storm:
            icon = &storm_96;
            break;

        default:
            break;
    }

    lv_img_set_src(iconImage, icon);

    // Scala l'icona (96x96) per riempire iconContainer mantenendo le proporzioni
    lv_img_set_zoom(iconImage, 256 * lv_obj_get_width(iconContainer) / icon->header.w);
    lv_img_set_antialias(iconImage, true);
}


void WeatherIconCard::refresh()
{
    auto& repo = HomeAssistantRepository::instance();

    std::string state = repo.getState("weather.openweathermap");
    Category category = categoryFromState(state);
    bool night = isNightTime();

    if(category != currentCategory || night != currentIsNight)
    {
        currentCategory = category;
        currentIsNight = night;
        buildIcon(category, night);
    }
}


void WeatherIconCard::timerCallback(lv_timer_t* timer)
{
    WeatherIconCard* self =
        static_cast<WeatherIconCard*>(
            lv_timer_get_user_data(timer)
        );

    if(self)
    {
        self->refresh();
    }
}