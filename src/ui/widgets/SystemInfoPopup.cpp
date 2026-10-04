#include "SystemInfoPopup.h"
#include "../../services/EventLog.h"
#include "../Theme.h"
#include "../Fonts.h"

#include <arpa/inet.h>
#include <ifaddrs.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/statvfs.h>

#include <cstdio>
#include <cstring>
#include <ctime>
#include <fstream>
#include <string>

namespace
{
    // ---- Letture di sistema ------------------------------------------------
    std::string localIp()
    {
        std::string out = "--";
        ifaddrs* list = nullptr;
        if(getifaddrs(&list) != 0) return out;

        for(ifaddrs* p = list; p; p = p->ifa_next)
        {
            if(!p->ifa_addr || p->ifa_addr->sa_family != AF_INET) continue;
            if(std::strcmp(p->ifa_name, "lo") == 0) continue;

            char buf[INET_ADDRSTRLEN];
            inet_ntop(AF_INET, &reinterpret_cast<sockaddr_in*>(p->ifa_addr)->sin_addr, buf, sizeof(buf));
            out = std::string(buf) + "  (" + p->ifa_name + ")";
            break;
        }
        freeifaddrs(list);
        return out;
    }

    float cpuTempC()
    {
        std::ifstream f("/sys/class/thermal/thermal_zone0/temp");
        long milli = 0;
        if(f >> milli) return milli / 1000.0f;
        return -1.0f;
    }

    // RAM usata / totale in MB (usata = MemTotal - MemAvailable)
    bool ramMb(double& used, double& total)
    {
        std::ifstream f("/proc/meminfo");
        std::string key, unit;
        long val = 0, memTotal = 0, memAvail = 0;
        while(f >> key >> val >> unit)
        {
            if(key == "MemTotal:")          memTotal = val;
            else if(key == "MemAvailable:") memAvail = val;
            if(memTotal && memAvail) break;
        }
        if(!memTotal) return false;
        total = memTotal / 1024.0;
        used  = (memTotal - memAvail) / 1024.0;
        return true;
    }

    // Spazio usato / totale della root (la SD) in GB
    bool sdGb(double& used, double& total)
    {
        struct statvfs s;
        if(statvfs("/", &s) != 0) return false;
        double fr = static_cast<double>(s.f_frsize);
        total = s.f_blocks * fr / 1e9;
        used  = (s.f_blocks - s.f_bfree) * fr / 1e9;
        return total > 0;
    }

    // ---- Helper UI ----------------------------------------------------------
    lv_obj_t* addRow(lv_obj_t* parent, const char* name)
    {
        lv_obj_t* r = lv_obj_create(parent);
        lv_obj_remove_style_all(r);
        lv_obj_set_size(r, LV_PCT(100), LV_SIZE_CONTENT);
        lv_obj_set_flex_flow(r, LV_FLEX_FLOW_ROW);
        lv_obj_set_flex_align(r, LV_FLEX_ALIGN_SPACE_BETWEEN, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
        lv_obj_set_style_pad_ver(r, 8, 0);

        lv_obj_t* n = lv_label_create(r);
        lv_label_set_text(n, name);
        lv_obj_set_style_text_font(n, &ui_sans_24, 0);
        lv_obj_set_style_text_color(n, Theme::c(Theme::MUTED), 0);

        lv_obj_t* v = lv_label_create(r);
        lv_label_set_text(v, "--");
        lv_obj_set_style_text_font(v, &ui_sansb_24, 0);
        lv_obj_set_style_text_color(v, Theme::c(Theme::TEXT), 0);
        return v;
    }
}


void SystemInfoPopup::create()
{
    // Fascia invisibile in alto, creata DOPO la griglia cosi' sta sopra (z-order)
    lv_obj_t* zone = lv_obj_create(lv_screen_active());
    lv_obj_remove_style_all(zone);
    lv_obj_set_size(zone, LV_PCT(100), 56);
    lv_obj_align(zone, LV_ALIGN_TOP_MID, 0, 0);
    lv_obj_add_flag(zone, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_event_cb(zone, onTap, LV_EVENT_CLICKED, this);
}

void SystemInfoPopup::onTap(lv_event_t* e)
{
    auto* self = static_cast<SystemInfoPopup*>(lv_event_get_user_data(e));
    if(!self->overlay) self->open();
}

void SystemInfoPopup::onClose(lv_event_t* e)
{
    static_cast<SystemInfoPopup*>(lv_event_get_user_data(e))->close();
}

void SystemInfoPopup::close()
{
    if(timer)   { lv_timer_delete(timer); timer = nullptr; }
    if(overlay) { lv_obj_delete_async(overlay); overlay = nullptr; }
}

void SystemInfoPopup::open()
{
    // Sfondo scuro a tutto schermo: un tap fuori dalla card chiude il popup
    overlay = lv_obj_create(lv_layer_top());
    lv_obj_remove_style_all(overlay);
    lv_obj_set_size(overlay, LV_PCT(100), LV_PCT(100));
    lv_obj_set_style_bg_color(overlay, lv_color_black(), 0);
    lv_obj_set_style_bg_opa(overlay, LV_OPA_60, 0);
    lv_obj_add_flag(overlay, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_event_cb(overlay, onClose, LV_EVENT_CLICKED, this);

    lv_obj_t* cd = lv_obj_create(overlay);
    lv_obj_set_width(cd, LV_PCT(88));
    lv_obj_set_height(cd, LV_SIZE_CONTENT);
    lv_obj_center(cd);
    lv_obj_set_flex_flow(cd, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_scrollable(cd, false);
    lv_obj_set_style_bg_color(cd, Theme::c(Theme::BG), 0);
    lv_obj_set_style_bg_opa(cd, LV_OPA_COVER, 0);
    lv_obj_set_style_border_width(cd, 2, 0);
    lv_obj_set_style_border_color(cd, Theme::c(Theme::CARD_LINE), 0);
    lv_obj_set_style_radius(cd, 24, 0);
    lv_obj_set_style_pad_all(cd, 28, 0);
    lv_obj_set_style_pad_row(cd, 4, 0);

    lv_obj_t* title = lv_label_create(cd);
    lv_label_set_text(title, "Informazioni sistema");
    lv_obj_set_style_text_font(title, &ui_sansb_28, 0);
    lv_obj_set_style_text_color(title, Theme::c(Theme::TEXT), 0);
    lv_obj_set_style_pad_bottom(title, 10, 0);

    ipL  = addRow(cd, "Indirizzo IP");
    cpuL = addRow(cd, "Temperatura CPU");
    ramL = addRow(cd, "RAM");
    sdL  = addRow(cd, "SD");

    lv_obj_t* logTitle = lv_label_create(cd);
    lv_label_set_text(logTitle, "Ultimi 10 eventi");
    lv_obj_set_style_text_font(logTitle, &ui_sansb_24, 0);
    lv_obj_set_style_text_color(logTitle, Theme::c(Theme::TEXT), 0);
    lv_obj_set_style_pad_top(logTitle, 16, 0);
    lv_obj_set_style_pad_bottom(logTitle, 6, 0);

    logL = lv_label_create(cd);
    lv_label_set_long_mode(logL, LV_LABEL_LONG_WRAP);
    lv_obj_set_width(logL, LV_PCT(100));
    lv_obj_set_style_text_font(logL, &ui_sans_20, 0);
    lv_obj_set_style_text_color(logL, Theme::c(Theme::TEXT), 0);

    lv_obj_t* btn = lv_obj_create(cd);
    lv_obj_set_size(btn, LV_PCT(100), 64);
    lv_obj_set_style_bg_color(btn, Theme::c(Theme::TEXT), 0);
    lv_obj_set_style_bg_opa(btn, LV_OPA_COVER, 0);
    lv_obj_set_style_radius(btn, 16, 0);
    lv_obj_set_style_border_width(btn, 0, 0);
    lv_obj_set_style_margin_top(btn, 16, 0);
    lv_obj_set_scrollable(btn, false);
    lv_obj_add_event_cb(btn, onClose, LV_EVENT_CLICKED, this);
    lv_obj_t* bl = lv_label_create(btn);
    lv_label_set_text(bl, "Chiudi");
    lv_obj_set_style_text_font(bl, &ui_sansb_24, 0);
    lv_obj_set_style_text_color(bl, Theme::c(Theme::WHITE), 0);
    lv_obj_center(bl);

    update();
    timer = lv_timer_create(
        [](lv_timer_t* t) { static_cast<SystemInfoPopup*>(lv_timer_get_user_data(t))->update(); },
        1000, this);
}

void SystemInfoPopup::update()
{
    if(!overlay) return;
    char b[96];

    lv_label_set_text(ipL, localIp().c_str());

    float t = cpuTempC();
    if(t >= 0) { std::snprintf(b, sizeof(b), "%.1f °C", t); lv_label_set_text(cpuL, b); }
    else         lv_label_set_text(cpuL, "--");

    double used, total;
    if(ramMb(used, total))
    {
        std::snprintf(b, sizeof(b), "%.0f / %.0f MB  (%.0f%%)", used, total, used / total * 100.0);
        lv_label_set_text(ramL, b);
    }
    else lv_label_set_text(ramL, "--");

    if(sdGb(used, total))
    {
        std::snprintf(b, sizeof(b), "%.1f / %.1f GB  (%.0f%%)", used, total, used / total * 100.0);
        lv_label_set_text(sdL, b);
    }
    else lv_label_set_text(sdL, "--");

    std::string log;
    for(const auto& e : EventLog::instance().snapshot())
    {
        std::tm tm = *std::localtime(&e.when);
        std::snprintf(b, sizeof(b), "%02d/%02d %02d:%02d:%02d  ", tm.tm_mday, tm.tm_mon + 1,
                      tm.tm_hour, tm.tm_min, tm.tm_sec);
        if(!log.empty()) log += "\n";
        log += b + e.text;
    }
    lv_label_set_text(logL, log.empty() ? "Nessun evento" : log.c_str());
}