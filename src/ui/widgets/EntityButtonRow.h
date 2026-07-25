#pragma once

#include <lvgl.h>
#include <string>
#include <vector>


struct EntityButtonConfig
{
    std::string entityId;   // es. "light.soggiorno"
    std::string domain;     // "light" o "switch"
    std::string label;      // testo mostrato, es. "Soggiorno"
};


class EntityButtonRow
{

public:

    void create(
        lv_obj_t* parent,
        int x,
        int y,
        const std::vector<EntityButtonConfig>& configs
    );

    // Aggiorna colore/stato di tutti i pulsanti leggendo dalla cache
    // del repository (nessun I/O di rete, non blocca mai).
    void refresh();


private:

    struct ButtonEntry
    {
        lv_obj_t* btn;
        lv_obj_t* label;
        EntityButtonConfig config;
    };

    std::vector<ButtonEntry> buttons;

    static void onClick(lv_event_t* e);
    void applyState(ButtonEntry& entry, bool isOn);

};
