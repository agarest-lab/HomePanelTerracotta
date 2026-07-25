#include "EntityButtonRow.h"
#include "../../services/HomeAssistantRepository.h"

#include <iostream>

#define BTN_WIDTH  195
#define BTN_HEIGHT 130
#define BTN_GAP    13


void EntityButtonRow::create(
    lv_obj_t* parent,
    int x,
    int y,
    const std::vector<EntityButtonConfig>& configs
)
{
    for(size_t i = 0; i < configs.size(); i++)
    {
        // Registra l'entità presso il repository, così il thread di
        // polling in background inizia a tenerne aggiornato lo stato.
        HomeAssistantRepository::instance().watch(configs[i].entityId);

        lv_obj_t* btn = lv_button_create(parent);

        lv_obj_set_size(btn, BTN_WIDTH, BTN_HEIGHT);

        lv_obj_set_pos(
            btn,
            x + (int)i * (BTN_WIDTH + BTN_GAP),
            y
        );

        lv_obj_set_style_radius(btn, 12, 0);

        lv_obj_t* label = lv_label_create(btn);
        lv_label_set_text(label, configs[i].label.c_str());
        lv_obj_set_style_text_font(label, &lv_font_montserrat_32, 0);
        lv_obj_center(label);

        ButtonEntry entry;
        entry.btn = btn;
        entry.label = label;
        entry.config = configs[i];

        buttons.push_back(entry);

        lv_obj_set_user_data(btn, this);
        lv_obj_add_event_cb(btn, onClick, LV_EVENT_CLICKED, (void*)(uintptr_t)i);
    }

    refresh();
}


void EntityButtonRow::applyState(ButtonEntry& entry, bool isOn)
{
    lv_obj_set_style_bg_color(
        entry.btn,
        isOn ? lv_color_hex(0xFFC107) : lv_color_hex(0x333333),
        0
    );
}


void EntityButtonRow::refresh()
{
    for(auto& entry : buttons)
    {
        bool isOn = HomeAssistantRepository::instance().getBoolState(entry.config.entityId);
        applyState(entry, isOn);
    }
}


void EntityButtonRow::onClick(lv_event_t* e)
{
    lv_obj_t* btn = (lv_obj_t*)lv_event_get_target(e);
    EntityButtonRow* self = static_cast<EntityButtonRow*>(lv_obj_get_user_data(btn));
    size_t index = (size_t)(uintptr_t)lv_event_get_user_data(e);

    auto& entry = self->buttons[index];

    bool isOn = HomeAssistantRepository::instance().getBoolState(entry.config.entityId);

    std::string service          = isOn ? "turn_off" : "turn_on";
    std::string optimisticState  = isOn ? "off"       : "on";

    // Non bloccante: il comando viene accodato ed eseguito dal thread
    // dedicato del repository. Il click torna subito.
    HomeAssistantRepository::instance().requestServiceCall(
        entry.config.domain,
        service,
        entry.config.entityId,
        optimisticState
    );

    // Feedback visivo immediato (ottimistico), senza aspettare la rete.
    self->applyState(entry, !isOn);
}
