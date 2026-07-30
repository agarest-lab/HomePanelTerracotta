#pragma once

#include <lvgl.h>
#include <string>
#include <vector>


struct ToggleConfig
{
    std::string entityId;
    std::string domain;
    std::string label;
};


class ToggleRow
{

public:

    void create(
        lv_obj_t* parent,
        int x,
        int y,
        const std::vector<ToggleConfig>& configs
    );

    void refresh();


private:

    struct ToggleEntry
    {
        lv_obj_t* sw = nullptr;
        lv_obj_t* label = nullptr;
        ToggleConfig config;
    };

    std::vector<ToggleEntry> toggles;

    static void onToggle(lv_event_t* e);

};
