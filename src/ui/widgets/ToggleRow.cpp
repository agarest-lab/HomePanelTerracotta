#include "ToggleRow.h"
#include "../../services/HomeAssistantRepository.h"


#define TOGGLE_WIDTH  205  //195
#define TOGGLE_GAP    13


void ToggleRow::create(
    lv_obj_t* parent,
    int x,
    int y,
    const std::vector<ToggleConfig>& configs
)
{
    for(size_t i = 0; i < configs.size(); i++)
    {
        HomeAssistantRepository::instance().watch(configs[i].entityId);
        lv_obj_t* container = lv_obj_create(parent);
        lv_obj_set_size(container, TOGGLE_WIDTH, 120); //90
        lv_obj_set_pos(container, x + (int)i * (TOGGLE_WIDTH + TOGGLE_GAP), y);
        // lv_obj_set_style_bg_color(container, lv_color_hex(0x1A1A1A), 0);
        lv_obj_set_style_bg_color(container, lv_color_hex(0x353535), 0);        // Colore iniziale
        lv_obj_set_style_bg_grad_color(container, lv_color_hex(0x171717), 0);  // Colore finale
        lv_obj_set_style_bg_grad_dir(container, LV_GRAD_DIR_RADIAL, 0);
        lv_obj_set_style_bg_opa(container, LV_OPA_COVER, 0);
        lv_obj_set_style_border_width(container, 0, 0);
        lv_obj_set_style_radius(container, 12, 0);
        lv_obj_set_style_pad_all(container, 8, 0);
        lv_obj_clear_flag(container, LV_OBJ_FLAG_SCROLLABLE);

        lv_obj_t* label = lv_label_create(container);
        lv_label_set_text(label, configs[i].label.c_str());
        lv_obj_set_style_text_color(label, lv_color_hex(0x23FF00), 0);
        lv_obj_set_style_text_font(label, &lv_font_montserrat_28, 0);
        lv_obj_align(label, LV_ALIGN_TOP_MID, 0, 0);

        lv_obj_t* sw = lv_switch_create(container);
        lv_obj_set_size(sw, 100, 50);
        lv_obj_align(sw, LV_ALIGN_BOTTOM_MID, 0, -6);

        lv_obj_set_style_bg_color(sw, lv_color_hex(0x07c1ff), LV_PART_INDICATOR | LV_STATE_CHECKED);

        ToggleEntry entry;
        entry.sw = sw;
        entry.label = label;
        entry.config = configs[i];

        toggles.push_back(entry);

        lv_obj_set_user_data(sw, this);
        lv_obj_add_event_cb(sw, onToggle, LV_EVENT_VALUE_CHANGED, (void*)(uintptr_t)i);
    }

    refresh();
}


void ToggleRow::refresh()
{
    for(auto& entry : toggles)
    {
        bool isOn = HomeAssistantRepository::instance().getBoolState(entry.config.entityId);

        if(isOn)
            lv_obj_add_state(entry.sw, LV_STATE_CHECKED);
        else
            lv_obj_remove_state(entry.sw, LV_STATE_CHECKED);
    }
}


void ToggleRow::onToggle(lv_event_t* e)
{
    lv_obj_t* sw = (lv_obj_t*)lv_event_get_target(e);
    ToggleRow* self = static_cast<ToggleRow*>(lv_obj_get_user_data(sw));
    size_t index = (size_t)(uintptr_t)lv_event_get_user_data(e);

    auto& entry = self->toggles[index];

    bool isNowOn = lv_obj_has_state(sw, LV_STATE_CHECKED);

    std::string service = isNowOn ? "turn_on" : "turn_off";
    std::string optimisticState = isNowOn ? "on" : "off";

    HomeAssistantRepository::instance().requestServiceCall(
        entry.config.domain,
        service,
        entry.config.entityId,
        optimisticState
    );
}
