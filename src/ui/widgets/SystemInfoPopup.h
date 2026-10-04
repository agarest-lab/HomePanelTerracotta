#pragma once
#include <lvgl.h>

class SystemInfoPopup
{
public:
    void create();                      // crea la zona di tap sullo schermo attivo

private:
    void open();
    void update();

    lv_obj_t*   overlay = nullptr;
    lv_obj_t*   ipL = nullptr;
    lv_obj_t*   cpuL = nullptr;
    lv_obj_t*   ramL = nullptr;
    lv_obj_t*   sdL = nullptr;
    lv_obj_t*   logL = nullptr;
    lv_timer_t* timer = nullptr;

    static void onTap(lv_event_t* e);
    static void onClose(lv_event_t* e);
    void close();
};