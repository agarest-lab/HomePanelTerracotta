#include "WashingMachineCard.h"
#include "../../services/HomeAssistantRepository.h"

#include <cstdio>


static const char* SWITCH_ENTITY = "switch.fvbalcone_presa_1";
static const char* POWER_SENSOR  = "sensor.fvbalcone_potenza";
static const float POWER_THRESHOLD = 20.0f;


void WashingMachineCard::create(
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

    lv_obj_set_style_bg_color(card, lv_color_hex(0x1B5E20), 0);   // verde di partenza
    lv_obj_set_style_bg_opa(card, LV_OPA_COVER, 0);
    lv_obj_set_style_border_width(card, 0, 0);
    lv_obj_set_style_radius(card, 12, 0);
    lv_obj_set_style_pad_all(card, 6, 0);

    lv_obj_clear_flag(card, LV_OBJ_FLAG_SCROLLABLE);


    // --- Icona: corpo (rettangolo arrotondato) ---

    int iconSize = height - 46;   // lascia spazio per la label sotto
    if(iconSize < 28) iconSize = 28;

    iconBody = lv_obj_create(card);
    lv_obj_set_size(iconBody, iconSize, iconSize);
    lv_obj_align(iconBody, LV_ALIGN_TOP_MID, 0, 4);
    lv_obj_set_style_bg_opa(iconBody, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(iconBody, 3, 0);
    lv_obj_set_style_border_color(iconBody, lv_color_white(), 0);
    lv_obj_set_style_radius(iconBody, 8, 0);
    lv_obj_clear_flag(iconBody, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_clear_flag(iconBody, LV_OBJ_FLAG_CLICKABLE);


    // --- Icona: oblò (cerchio) ---

    int drumSize = (int)(iconSize * 0.6f);

    iconDrum = lv_obj_create(iconBody);
    lv_obj_set_size(iconDrum, drumSize, drumSize);
    lv_obj_center(iconDrum);
    lv_obj_set_style_bg_opa(iconDrum, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(iconDrum, 3, 0);
    lv_obj_set_style_border_color(iconDrum, lv_color_white(), 0);
    lv_obj_set_style_radius(iconDrum, LV_RADIUS_CIRCLE, 0);
    lv_obj_clear_flag(iconDrum, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_clear_flag(iconDrum, LV_OBJ_FLAG_CLICKABLE);


    // --- Icona: pallino di blocco/spia in alto a sinistra ---

    int lockSize = (int)(iconSize * 0.12f);
    if(lockSize < 5) lockSize = 5;

    iconDoorLock = lv_obj_create(iconBody);
    lv_obj_set_size(iconDoorLock, lockSize, lockSize);
    lv_obj_align(iconDoorLock, LV_ALIGN_TOP_LEFT, 4, 4);
    lv_obj_set_style_bg_color(iconDoorLock, lv_color_white(), 0);
    lv_obj_set_style_bg_opa(iconDoorLock, LV_OPA_COVER, 0);
    lv_obj_set_style_border_width(iconDoorLock, 0, 0);
    lv_obj_set_style_radius(iconDoorLock, LV_RADIUS_CIRCLE, 0);
    lv_obj_clear_flag(iconDoorLock, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_clear_flag(iconDoorLock, LV_OBJ_FLAG_CLICKABLE);


    // --- Valore di potenza al posto della label testuale ---

    valueLabel = lv_label_create(card);
    lv_obj_set_style_text_color(valueLabel, lv_color_white(), 0);
    lv_obj_set_style_text_font(valueLabel, &lv_font_montserrat_28, 0);
    lv_obj_align(valueLabel, LV_ALIGN_BOTTOM_MID, 0, 0);


    auto& repo = HomeAssistantRepository::instance();
    repo.watch(SWITCH_ENTITY);
    repo.watch(POWER_SENSOR);


    // lv_obj_add_flag(card, LV_OBJ_FLAG_CLICKABLE);
    // lv_obj_add_event_cb(card, onClick, LV_EVENT_CLICKED, this);


    refresh();

    updateTimer = lv_timer_create(timerCallback, 2000, this);
}


void WashingMachineCard::refresh()
{
    auto& repo = HomeAssistantRepository::instance();

    float power = repo.getFloatState(POWER_SENSOR);

    char buf[24];
    std::snprintf(buf, sizeof(buf), "%d W", (int)power);
    lv_label_set_text(valueLabel, buf);

    if(power > POWER_THRESHOLD)
        lv_obj_set_style_bg_color(card, lv_color_hex(0x5E1B1B), 0);   // rosso
    else
        lv_obj_set_style_bg_color(card, lv_color_hex(0x1B5E20), 0);   // verde
}


/*
void WashingMachineCard::onClick(lv_event_t* e)
{
    WashingMachineCard* self =
        static_cast<WashingMachineCard*>(
            lv_event_get_user_data(e)
        );

    if(!self)
        return;

    auto& repo = HomeAssistantRepository::instance();

    bool isOn = repo.getBoolState(SWITCH_ENTITY);

    std::string service = isOn ? "turn_off" : "turn_on";
    std::string optimisticState = isOn ? "off" : "on";

    repo.requestServiceCall("switch", service, SWITCH_ENTITY, optimisticState);
}
*/

void WashingMachineCard::timerCallback(lv_timer_t* timer)
{
    WashingMachineCard* self =
        static_cast<WashingMachineCard*>(
            lv_timer_get_user_data(timer)
        );

    if(self)
    {
        self->refresh();
    }
}
