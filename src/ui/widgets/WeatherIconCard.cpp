#include "WeatherIconCard.h"
#include "../../services/HomeAssistantRepository.h"

#include <algorithm>


// --- Wrapper per animare l'opacità (la firma di lv_obj_set_style_bg_opa
// non combacia direttamente con lv_anim_exec_xcb_t, serve un adattatore) ---

static void animSetBgOpa(void* obj, int32_t value)
{
    lv_obj_set_style_bg_opa(static_cast<lv_obj_t*>(obj), (lv_opa_t)value, 0);
}


WeatherIconCard::Category WeatherIconCard::categoryFromState(const std::string& state)
{
    if(state == "sunny" || state == "clear-night")
        return Category::Clear;

    if(state == "cloudy" || state == "partlycloudy" ||
       state == "fog" || state == "windy" || state == "windy-variant")
        return Category::Cloudy;

    if(state == "rainy" || state == "pouring" || state == "snowy" ||
       state == "snowy-rainy" || state == "hail")
        return Category::Rain;

    if(state == "lightning" || state == "lightning-rainy" || state == "exceptional")
        return Category::Storm;

    return Category::Cloudy;   // fallback ragionevole
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

    lv_obj_set_style_bg_color(card, lv_color_hex(0x1A1A1A), 0);
    lv_obj_set_style_bg_opa(card, LV_OPA_COVER, 0);
    lv_obj_set_style_border_width(card, 0, 0);
    lv_obj_set_style_radius(card, 16, 0);
    lv_obj_set_style_pad_all(card, 0, 0);

    lv_obj_clear_flag(card, LV_OBJ_FLAG_SCROLLABLE);

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


    auto& repo = HomeAssistantRepository::instance();
    repo.watch("weather.openweathermap");


    refresh();

    updateTimer = lv_timer_create(timerCallback, 5000, this);
}


void WeatherIconCard::addCloudShape(lv_color_t color)
{
    int s = lv_obj_get_width(iconContainer);

    // Corpo della nuvola: rettangolo arrotondato largo, non troppo alto
    lv_obj_t* body = lv_obj_create(iconContainer);
    lv_obj_set_size(body, (int)(s * 0.8f), (int)(s * 0.38f));
    lv_obj_align(body, LV_ALIGN_BOTTOM_MID, 0, -(int)(s * 0.08f));
    lv_obj_set_style_bg_color(body, color, 0);
    lv_obj_set_style_bg_opa(body, LV_OPA_COVER, 0);
    lv_obj_set_style_border_width(body, 0, 0);
    lv_obj_set_style_radius(body, LV_RADIUS_CIRCLE, 0);
    lv_obj_clear_flag(body, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_clear_flag(body, LV_OBJ_FLAG_CLICKABLE);

    // Due "gobbe" della nuvola (cerchi) sopra il corpo
    lv_obj_t* bump1 = lv_obj_create(iconContainer);
    lv_obj_set_size(bump1, (int)(s * 0.42f), (int)(s * 0.42f));
    lv_obj_align_to(bump1, body, LV_ALIGN_TOP_LEFT, (int)(s * 0.06f), (int)(s * 0.08f));
    lv_obj_set_style_bg_color(bump1, color, 0);
    lv_obj_set_style_bg_opa(bump1, LV_OPA_COVER, 0);
    lv_obj_set_style_border_width(bump1, 0, 0);
    lv_obj_set_style_radius(bump1, LV_RADIUS_CIRCLE, 0);
    lv_obj_clear_flag(bump1, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_clear_flag(bump1, LV_OBJ_FLAG_CLICKABLE);

    lv_obj_t* bump2 = lv_obj_create(iconContainer);
    lv_obj_set_size(bump2, (int)(s * 0.5f), (int)(s * 0.5f));
    lv_obj_align_to(bump2, body, LV_ALIGN_TOP_RIGHT, -(int)(s * 0.1f), 0);
    lv_obj_set_style_bg_color(bump2, color, 0);
    lv_obj_set_style_bg_opa(bump2, LV_OPA_COVER, 0);
    lv_obj_set_style_border_width(bump2, 0, 0);
    lv_obj_set_style_radius(bump2, LV_RADIUS_CIRCLE, 0);
    lv_obj_clear_flag(bump2, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_clear_flag(bump2, LV_OBJ_FLAG_CLICKABLE);
}


void WeatherIconCard::buildClearIcon()
{
    int s = lv_obj_get_width(iconContainer);

    lv_obj_t* sun = lv_obj_create(iconContainer);
    int sunSize = (int)(s * 0.55f);
    lv_obj_set_size(sun, sunSize, sunSize);
    lv_obj_center(sun);
    lv_obj_set_style_bg_color(sun, lv_color_hex(0xFFCA28), 0);
    lv_obj_set_style_bg_opa(sun, LV_OPA_COVER, 0);
    lv_obj_set_style_border_width(sun, 0, 0);
    lv_obj_set_style_radius(sun, LV_RADIUS_CIRCLE, 0);
    lv_obj_clear_flag(sun, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_clear_flag(sun, LV_OBJ_FLAG_CLICKABLE);

    // Animazione "respiro": l'opacità oscilla dolcemente
    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_var(&a, sun);
    lv_anim_set_exec_cb(&a, animSetBgOpa);
    lv_anim_set_values(&a, 150, 255);
    lv_anim_set_time(&a, 1200);
    lv_anim_set_playback_time(&a, 1200);
    lv_anim_set_repeat_count(&a, LV_ANIM_REPEAT_INFINITE);
    lv_anim_start(&a);
}


void WeatherIconCard::buildCloudyIcon()
{
    addCloudShape(lv_color_hex(0xB0BEC5));

    // Leggera deriva orizzontale della nuvola
    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_var(&a, iconContainer);
    lv_anim_set_exec_cb(&a, (lv_anim_exec_xcb_t)lv_obj_set_x);
    lv_anim_set_values(&a, lv_obj_get_x(iconContainer) - 8, lv_obj_get_x(iconContainer) + 8);
    lv_anim_set_time(&a, 2200);
    lv_anim_set_playback_time(&a, 2200);
    lv_anim_set_repeat_count(&a, LV_ANIM_REPEAT_INFINITE);
    lv_anim_start(&a);
}


void WeatherIconCard::buildRainIcon()
{
    addCloudShape(lv_color_hex(0x78909C));

    int s = lv_obj_get_width(iconContainer);
    int dropSize = (int)(s * 0.09f);
    if(dropSize < 4) dropSize = 4;

    int startY = (int)(s * 0.55f);
    int endY = s - dropSize;

    for(int i = 0; i < 3; i++)
    {
        lv_obj_t* drop = lv_obj_create(iconContainer);
        lv_obj_set_size(drop, dropSize, dropSize * 2);
        lv_obj_set_style_bg_color(drop, lv_color_hex(0x4FC3F7), 0);
        lv_obj_set_style_bg_opa(drop, LV_OPA_COVER, 0);
        lv_obj_set_style_border_width(drop, 0, 0);
        lv_obj_set_style_radius(drop, LV_RADIUS_CIRCLE, 0);
        lv_obj_clear_flag(drop, LV_OBJ_FLAG_SCROLLABLE);
        lv_obj_clear_flag(drop, LV_OBJ_FLAG_CLICKABLE);

        int dropX = (int)(s * (0.28f + i * 0.22f));
        lv_obj_set_pos(drop, dropX, startY);

        lv_anim_t a;
        lv_anim_init(&a);
        lv_anim_set_var(&a, drop);
        lv_anim_set_exec_cb(&a, (lv_anim_exec_xcb_t)lv_obj_set_y);
        lv_anim_set_values(&a, startY, endY);
        lv_anim_set_time(&a, 700);
        lv_anim_set_delay(&a, i * 220);
        lv_anim_set_repeat_count(&a, LV_ANIM_REPEAT_INFINITE);
        lv_anim_set_repeat_delay(&a, 200);
        lv_anim_start(&a);
    }
}


void WeatherIconCard::buildStormIcon()
{
    addCloudShape(lv_color_hex(0x546E7A));

    int s = lv_obj_get_width(iconContainer);

    lv_obj_t* bolt = lv_obj_create(iconContainer);
    int boltW = (int)(s * 0.14f);
    int boltH = (int)(s * 0.42f);
    lv_obj_set_size(bolt, boltW, boltH);
    lv_obj_align(bolt, LV_ALIGN_BOTTOM_MID, 0, -(int)(s * 0.05f));
    lv_obj_set_style_bg_color(bolt, lv_color_hex(0xFFEB3B), 0);
    lv_obj_set_style_bg_opa(bolt, LV_OPA_COVER, 0);
    lv_obj_set_style_border_width(bolt, 0, 0);
    lv_obj_set_style_radius(bolt, 3, 0);
    lv_obj_clear_flag(bolt, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_clear_flag(bolt, LV_OBJ_FLAG_CLICKABLE);

    // Lampeggio: opacità che scatta tra trasparente e piena
    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_var(&a, bolt);
    lv_anim_set_exec_cb(&a, animSetBgOpa);
    lv_anim_set_values(&a, 60, 255);
    lv_anim_set_time(&a, 120);
    lv_anim_set_playback_time(&a, 120);
    lv_anim_set_repeat_count(&a, LV_ANIM_REPEAT_INFINITE);
    lv_anim_set_repeat_delay(&a, 900);
    lv_anim_start(&a);
}


void WeatherIconCard::buildIcon(Category category)
{
    lv_obj_clean(iconContainer);

    switch(category)
    {
        case Category::Clear:  buildClearIcon();  break;
        case Category::Cloudy: buildCloudyIcon(); break;
        case Category::Rain:   buildRainIcon();   break;
        case Category::Storm:  buildStormIcon();  break;
        default: break;
    }
}


void WeatherIconCard::refresh()
{
    auto& repo = HomeAssistantRepository::instance();

    std::string state = repo.getState("weather.openweathermap");
    Category category = categoryFromState(state);

    // Ricostruisce l'icona (e riavvia l'animazione) SOLO se la
    // categoria è cambiata, per non "spezzare" l'animazione ogni
    // pochi secondi durante il refresh periodico.
    if(category != currentCategory)
    {
        currentCategory = category;
        buildIcon(category);
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
