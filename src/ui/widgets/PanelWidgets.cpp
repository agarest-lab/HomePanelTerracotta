#include "PanelWidgets.h"
#include "../Theme.h"
#include "../Fonts.h"
#include "../Icons.h"
#include "../../services/HomeAssistantRepository.h"
#include "../../services/CalendarFeed.h"

#include <algorithm>
#include <cctype>
#include <cmath>
#include <cstdio>
#include <ctime>

#include "../../services/EventLog.h"

using namespace Theme;

// ============================================================================
//  Helper locali
// ============================================================================
namespace
{
    HomeAssistantRepository& repo() { return HomeAssistantRepository::instance(); }

    template<typename... A>
    void setf(lv_obj_t* l, const char* fmt, A... a)
    {
        char b[160];
        std::snprintf(b, sizeof(b), fmt, a...);
        lv_label_set_text(l, b);
    }

    const lv_font_t* S18 = &ui_sans_18;  const lv_font_t* S20 = &ui_sans_20;
    const lv_font_t* S22 = &ui_sans_22;  const lv_font_t* S24 = &ui_sans_24;
    const lv_font_t* S26 = &ui_sans_26;  const lv_font_t* S28 = &ui_sans_28;
    const lv_font_t* B22 = &ui_sansb_22; const lv_font_t* B24 = &ui_sansb_24; const lv_font_t* B28 = &ui_sansb_28;
    const lv_font_t* R36 = &ui_serif_36; const lv_font_t* R44 = &ui_serif_44;
    const lv_font_t* R52 = &ui_serif_52; const lv_font_t* R60 = &ui_serif_60;
    const lv_font_t* I24 = &ui_icons_24; const lv_font_t* I40 = &ui_icons_40; const lv_font_t* I72 = &ui_icons_72;

    const char* kAlertEntity = "binary_sensor.meteoalarm_friuli_venezia_giulia";

    const char* kGiorni[7] = { "Domenica", "Lunedì", "Martedì", "Mercoledì", "Giovedì", "Venerdì", "Sabato" };
    const char* kMesi[12]  = { "gennaio", "febbraio", "marzo", "aprile", "maggio", "giugno",
                               "luglio", "agosto", "settembre", "ottobre", "novembre", "dicembre" };

    float pct(float v, float max) { return std::clamp(v / max * 100.0f, 0.0f, 100.0f); }

    // ---- Icone meteo (stessa logica di WeatherIconCard) -------------------
    bool isNight()
    {
        if(repo().getState("sun.sun") == "below_horizon") return true;
        return repo().getState("weather.openweathermap") == "clear-night";
    }

    const lv_img_dsc_t* weatherIcon()
    {
        std::string s = repo().getState("weather.openweathermap");
        bool n = isNight();

        if(s == "sunny" || s == "clear-night")                                   return n ? &moon_96 : &sun_96;
        if(s == "windy" || s == "windy-variant")                                 return &wind_96;
        if(s == "rainy" || s == "pouring" || s == "hail" || s == "snowy-rainy")  return &rain_96;
        if(s == "snowy")                                                         return &snow_96;
        if(s == "lightning" || s == "lightning-rainy" || s == "exceptional")     return &storm_96;
        return n ? &moon_cloud_96 : &sun_cloud_96;
    }

    void showIcon(lv_obj_t* img, const lv_img_dsc_t* d, int size)
    {
        lv_img_set_src(img, d);
        lv_img_set_zoom(img, 256 * size / d->header.w);
        lv_img_set_antialias(img, true);
    }

    // Slot per l'icona meteo: l'immagine e' mostrata a dimensione nativa (96 px)
    // dentro uno slot piu' grande, cosi' non viene mai ritagliata.
    lv_obj_t* iconSlot(lv_obj_t* parent, int size)
    {
        lv_obj_t* slot = box(parent, LV_FLEX_FLOW_ROW, size, size);
        lv_obj_set_style_clip_corner(slot, false, 0);
        lv_obj_t* img = lv_img_create(slot);
        lv_obj_center(img);
        return img;
    }

    // ---- Toni dinamici (stessa logica di prima, nuova palette) -------------
    Tone toneMeteo(float t)
    {
        if(t < 0)  return { 0x3C6E8F, false };
        if(t < 5)  return { 0x4F8FA3, false };
        if(t < 15) return { TEAL,     false };
        if(t < 25) return { MUSTARD,  true  };
        if(t < 32) return { ORANGE,   false };
        return { BRICK, false };
    }

    Tone toneAqi(float a)
    {
        if(a <= 50)  return { OLIVE,    false };
        if(a <= 100) return { MUSTARD,  true  };
        if(a <= 150) return { ORANGE,   false };
        if(a <= 200) return { RED,      false };
        if(a <= 300) return { 0x6B4A7A, false };
        return { 0x5A2E1A, false };
    }

    const char* aqiText(float a)
    {
        if(a <= 50)  return "Ottima";
        if(a <= 100) return "Moderata";
        if(a <= 150) return "Scadente";
        if(a <= 200) return "Cattiva";
        if(a <= 300) return "Molto cattiva";
        return "Pericolosa";
    }

    std::string pollutantLabel(const std::string& p)
    {
        if(p.empty() || p == "unknown" || p == "unavailable") return "Inq --";
        std::string u;
        for(char ch : p) if(ch != '.') u += static_cast<char>(std::toupper(static_cast<unsigned char>(ch)));
        if(u == "PM25") u = "PM2.5";
        return "Inq " + u;
    }

    // "2026-08-03T21:09:00..." -> "03/08/2026 21:09"
    std::string fmtDateTime(const std::string& raw)
    {
        if(raw.size() < 16 || raw[10] != 'T') return raw;
        return raw.substr(8, 2) + "/" + raw.substr(5, 2) + "/" + raw.substr(0, 4) + " " + raw.substr(11, 5);
    }

    bool isToday(const CalendarEvent& ev)
    {
        if(ev.startIso.size() < 10) return false;
        std::time_t now = std::time(nullptr);
        std::tm t = *std::localtime(&now);
        char buf[11];
        std::strftime(buf, sizeof(buf), "%Y-%m-%d", &t);
        return ev.startIso.compare(0, 10, buf) == 0;
    }

    uint32_t calendarColor(const std::string& id)
    {
        if(id == "calendar.omar_rossoni_gmail_com") return TEAL_T;
        if(id == "calendar.sendina")                return TERRA_T;
        return TEXT;
    }

    // Intestazione di una card: icona + titolo
    lv_obj_t* headRow(lv_obj_t* card, const char* icon, const char* title, int32_t bottom = 12)
    {
        lv_obj_t* r = box(card, LV_FLEX_FLOW_ROW, LV_PCT(100));
        lv_obj_set_flex_align(r, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
        lv_obj_set_style_pad_column(r, 10, 0);
        lv_obj_set_style_pad_bottom(r, bottom, 0);
        label(r, icon, I24, TEXT);
        label(r, title, B24, TEXT);
        return r;
    }

    // Tessera interna (valore grande + sottotitolo) per Clima / Sistema / Temperature
    template<typename M>
    void makeM(lv_obj_t* parent, M& m, const char* icon, const char* title,
               const lv_font_t* valueFont, const lv_font_t* subFont)
    {
        m.obj = lv_obj_create(parent);
        lv_obj_set_size(m.obj, 0, LV_PCT(100));
        lv_obj_set_flex_grow(m.obj, 1);
        lv_obj_set_flex_flow(m.obj, LV_FLEX_FLOW_COLUMN);
        lv_obj_set_flex_align(m.obj, LV_FLEX_ALIGN_SPACE_BETWEEN, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START);
        lv_obj_set_style_border_width(m.obj, 0, 0);
        lv_obj_set_style_radius(m.obj, 20, 0);
        lv_obj_set_style_pad_hor(m.obj, 16, 0);
        lv_obj_set_style_pad_ver(m.obj, 8, 0);
        lv_obj_set_style_pad_row(m.obj, 0, 0);
        lv_obj_set_scrollable(m.obj, false);
        lv_obj_set_style_bg_opa(m.obj, LV_OPA_COVER, 0);

        if(icon)
        {
            lv_obj_t* r = box(m.obj, LV_FLEX_FLOW_ROW);
            lv_obj_set_flex_align(r, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
            lv_obj_set_style_pad_column(r, 8, 0);
            m.icon = label(r, icon, I24);
            m.title = label(r, title, S20);
        }
        else
            m.title = label(m.obj, title, S20);

        m.value = label(m.obj, "--", valueFont);
        m.sub = label(m.obj, "", subFont);
    }

    template<typename M>
    void paintM(M& m, Tone t)
    {
        tint(m.obj, t);
        if(m.icon) ink(m.icon, t);
        ink(m.title, t);
        ink(m.value, t);
        ink(m.sub, t, true);
    }
}


void Panel::autoRefresh(uint32_t ms)
{
    lv_timer_create(
        [](lv_timer_t* t) { static_cast<Panel*>(lv_timer_get_user_data(t))->refresh(); },
        ms, this);
}


// ============================================================================
//  ALLERTA METEO (colore per livello, come WeatherCardsRow)
// ============================================================================
void AlertBanner::create(lv_obj_t* cell)
{
    obj = tile(cell, OLIVE);
    lv_obj_set_flex_flow(obj, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(obj, LV_FLEX_ALIGN_SPACE_BETWEEN, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_ver(obj, 0, 0);
    lv_obj_set_style_pad_hor(obj, 28, 0);

    lv_obj_t* l = box(obj, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(l, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_column(l, 16, 0);
    icon  = label(l, Ico::ALERT, I40);
    title = label(l, "Allerta meteo", S22);
    event = label(l, "", B28);
    info  = label(obj, "", S28);

    repo().watch(kAlertEntity);
    refresh();
    autoRefresh(5000);
}

void AlertBanner::refresh()
{
    std::string state   = repo().getState(kAlertEntity);
    std::string level   = repo().getAttribute(kAlertEntity, "awareness_level");
    std::string ev      = repo().getAttribute(kAlertEntity, "event");
    std::string expires = repo().getAttribute(kAlertEntity, "expires");

    size_t semi = level.find(';');                        // "2; yellow" -> "2"
    if(semi != std::string::npos) level = level.substr(0, semi);
    level.erase(std::remove(level.begin(), level.end(), ' '), level.end());

    bool noAlert = (state == "off" || level == "0" || level.empty());

    std::string lower = ev;
    std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
    std::string text = "Nessuna";
    if(!noAlert)
    {
        text = "Allerta";
        if(lower.find("rain") != std::string::npos)             text = "Pioggia";
        else if(lower.find("storm") != std::string::npos)       text = "Temporali";
        else if(lower.find("wind") != std::string::npos)        text = "Vento";
        else if(lower.find("snow") != std::string::npos)        text = "Neve";
        else if(lower.find("temperature") != std::string::npos) text = "Temperatura";
        else if(!ev.empty())                                    text = ev;
    }
    lv_label_set_text(event, text.c_str());
    
    static std::string lastAlert;
    if(text != lastAlert)
    {
        if(!lastAlert.empty()) EventLog::instance().add("Allerta meteo: " + text);
        lastAlert = text;
    }

    if(noAlert || expires.empty())
        lv_label_set_text(info, "Non presente");
    else
    {
        size_t tp = expires.find('T');
        std::string hhmm = (tp != std::string::npos && tp + 6 <= expires.size()) ? expires.substr(tp + 1, 5) : "";
        setf(info, "Fino alle %s", hhmm.empty() ? "--:--" : hhmm.c_str());
    }

    Tone t = noAlert ? Tone{ OLIVE, false } : level == "2" ? Tone{ MUSTARD, true } :
             level == "3" ? Tone{ ORANGE, false } : level == "4" ? Tone{ RED, false } : Tone{ 0x5A2E1A, false };
    tint(obj, t);
    for(lv_obj_t* l : { icon, title, event, info }) ink(l, t);
}


// ============================================================================
//  OROLOGIO + METEO ATTUALE
// ============================================================================
void ClockBar::create(lv_obj_t* cell)
{
    lv_obj_t* row = box(cell, LV_FLEX_FLOW_ROW, LV_PCT(100), LV_PCT(100));
    lv_obj_set_flex_align(row, LV_FLEX_ALIGN_SPACE_BETWEEN, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_hor(row, 8, 0);

    lv_obj_t* l = box(row, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(l, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_column(l, 22, 0);
    timeL = label(l, "--:--", &ui_clock_112);
    dateL = label(l, "", S28);

    lv_obj_t* r = box(row, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(r, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_column(r, 14, 0);
    icon = iconSlot(r, 104);
    lv_obj_t* rt = box(r, LV_FLEX_FLOW_COLUMN);
    tempL = label(rt, "-- °C", R60);
    condL = label(rt, "", S24, MUTED);

    repo().watch("sensor.openweathermap_temperature");
    repo().watch("sensor.openweathermap_weather");
    repo().watch("weather.openweathermap");
    repo().watch("sun.sun");

    refresh();
    autoRefresh(1000);
}

void ClockBar::refresh()
{
    std::time_t now = std::time(nullptr);
    std::tm t = *std::localtime(&now);

    setf(timeL, "%02d:%02d", t.tm_hour, t.tm_min);
    setf(dateL, "%s\n%d %s %d", kGiorni[t.tm_wday], t.tm_mday, kMesi[t.tm_mon], t.tm_year + 1900);

    setf(tempL, "%.1f °C", repo().getFloatState("sensor.openweathermap_temperature"));
    std::string cond = repo().getState("sensor.openweathermap_weather");
    lv_label_set_text(condL, cond.empty() ? "--" : cond.c_str());

    const lv_img_dsc_t* d = weatherIcon();
    if(d != shownIcon) { shownIcon = d; showIcon(icon, d, 96); }
}


// ============================================================================
//  ENERGIA (4 tessere): stessa logica di EnergyGaugesPanel, nuova palette
// ============================================================================
void EnergyTile::create(lv_obj_t* cell, Kind k)
{
    kind = k;
    const char* ic = Ico::HOME;
    const char* title = "Consumo casa";
    switch(kind)
    {
        case Kind::House:   ic = Ico::HOME;      title = "Consumo casa"; break;
        case Kind::Pv:      ic = Ico::SUN;       title = "Fotovoltaico"; break;
        case Kind::Battery: ic = Ico::BATT_2;    title = "Batteria";     break;
        case Kind::Grid:    ic = Ico::TRANSFORM; title = "Rete";         break;
    }

    obj = tile(cell, TEAL);
    lv_obj_set_flex_flow(obj, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(obj, LV_FLEX_ALIGN_SPACE_BETWEEN, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START);

    lv_obj_t* head = box(obj, LV_FLEX_FLOW_ROW, LV_PCT(100));
    lv_obj_set_flex_align(head, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_column(head, 10, 0);
    iconL  = label(head, ic, I24);
    titleL = label(head, title, B24);

    valueL = label(obj, "--", R52);

    bar = lv_bar_create(obj);
    lv_obj_set_size(bar, LV_PCT(100), 12);
    lv_bar_set_range(bar, 0, 100);
    lv_obj_set_style_radius(bar, 6, LV_PART_MAIN);
    lv_obj_set_style_radius(bar, 6, LV_PART_INDICATOR);

    lv_obj_t* foot = box(obj, LV_FLEX_FLOW_ROW, LV_PCT(100));
    lv_obj_set_flex_align(foot, LV_FLEX_ALIGN_SPACE_BETWEEN, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    subL = label(foot, "", S22);
    subR = label(foot, "", S22);

    switch(kind)
    {
        case Kind::House:
            repo().watch("sensor.potenza_somma_casa");
            repo().watch("sensor.1p7k_new_potenza");
            break;
        case Kind::Pv:
            repo().watch("sensor.inverter_potenza_attiva");
            repo().watch("sensor.inverter_rendimento_giornaliero");
            repo().watch("sensor.inverter_internal_temperature");
            break;
        case Kind::Battery:
            repo().watch("sensor.batterie_stato_della_capacita");
            repo().watch("sensor.batterie_potenza_di_carica_scarica");
            repo().watch("sensor.batteria_1_temperatura_bms");
            break;
        case Kind::Grid:
            repo().watch("sensor.contatore_di_energia_potenza_attiva");
            repo().watch("sensor.en_importata_sun_oggi");
            repo().watch("sensor.en_immessa_sun_oggi");
            break;
    }

    refresh();
    autoRefresh(3000);
}

void EnergyTile::apply(float barPct, bool light, uint32_t bg)
{
    Tone t{ bg, light };
    tint(obj, t);
    for(lv_obj_t* l : { iconL, titleL, valueL }) ink(l, t);
    for(lv_obj_t* l : { subL, subR }) ink(l, t, true);

    lv_bar_set_value(bar, static_cast<int>(barPct), LV_ANIM_OFF);
    if(light)
    {
        lv_obj_set_style_bg_color(bar, c(SAND_D), LV_PART_MAIN);
        lv_obj_set_style_bg_opa(bar, LV_OPA_COVER, LV_PART_MAIN);
        lv_obj_set_style_bg_color(bar, c(INK), LV_PART_INDICATOR);
    }
    else
    {
        lv_obj_set_style_bg_color(bar, c(WHITE), LV_PART_MAIN);
        lv_obj_set_style_bg_opa(bar, 82, LV_PART_MAIN);              // ~32%
        lv_obj_set_style_bg_color(bar, c(WHITE), LV_PART_INDICATOR);
    }
}

void EnergyTile::refresh()
{
    switch(kind)
    {
    case Kind::House:
    {
        float w  = repo().getFloatState("sensor.potenza_somma_casa");
        float kw = repo().getFloatState("sensor.1p7k_new_potenza");
        setf(valueL, "%d W", (int)w);
        lv_label_set_text(subL, "Consumo attuale");
        setf(subR, "%.1f kW", kw);
        apply(pct(w, 6000), false, TEAL);
        break;
    }
    case Kind::Pv:
    {
        float w   = repo().getFloatState("sensor.inverter_potenza_attiva");
        float day = repo().getFloatState("sensor.inverter_rendimento_giornaliero");
        float t   = repo().getFloatState("sensor.inverter_internal_temperature");
        setf(valueL, "%d W", (int)w);
        setf(subL, "INV %.0f °C", t);
        setf(subR, "Oggi %.1f kWh", day);
        if(w > 20) apply(pct(w, 6000), true, MUSTARD);      // produce
        else       apply(0, false, SAND);                    // a riposo
        break;
    }
    case Kind::Battery:
    {
        float soc = repo().getFloatState("sensor.batterie_stato_della_capacita");
        float w   = repo().getFloatState("sensor.batterie_potenza_di_carica_scarica");
        float bms = repo().getFloatState("sensor.batteria_1_temperatura_bms");

        setf(valueL, "%d%%", (int)soc);
        setf(subL, "BMS %.0f °C", bms);
        if(w > 5)       setf(subR, "Carica %d W", (int)w);
        else if(w < -5) setf(subR, "Scarica %d W", (int)-w);
        else            lv_label_set_text(subR, "In equilibrio");

        lv_label_set_text(iconL, soc >= 88 ? Ico::BATT_4 : soc >= 63 ? Ico::BATT_3 :
                                 soc >= 38 ? Ico::BATT_2 : soc >= 13 ? Ico::BATT_1 : Ico::BATT_0);

        if(soc < 20)      apply(soc, false, BRICK);
        else if(soc < 50) apply(soc, true, MUSTARD);
        else              apply(soc, false, OLIVE);
        break;
    }
    case Kind::Grid:
    {
        float w   = repo().getFloatState("sensor.contatore_di_energia_potenza_attiva");
        float imp = repo().getFloatState("sensor.en_importata_sun_oggi");
        float exp = repo().getFloatState("sensor.en_immessa_sun_oggi");

        setf(valueL, "%d W", (int)std::fabs(w));
        setf(subL, "Prel %.1f kWh", imp);
        setf(subR, "Imm %.1f kWh", exp);

        if(w > 10)       { lv_label_set_text(titleL, "Rete · in immissione"); apply(pct(std::fabs(w), 5000), false, OLIVE);  }
        else if(w < -10) { lv_label_set_text(titleL, "Rete · in prelievo");   apply(pct(std::fabs(w), 5000), false, ORANGE); }
        else             { lv_label_set_text(titleL, "Rete · in equilibrio"); apply(0, false, SAND); }
        break;
    }
    }
}


// ============================================================================
//  TELECAMERA
// ============================================================================
void CameraPanel::create(lv_obj_t* cell, const std::string& entityId)
{
    lv_obj_t* t = tile(cell, CAM);
    lv_obj_set_flex_flow(t, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_pad_row(t, 10, 0);
    lv_obj_set_style_pad_hor(t, 18, 0);

    lv_obj_t* head = box(t, LV_FLEX_FLOW_ROW, LV_PCT(100));
    lv_obj_set_flex_align(head, LV_FLEX_ALIGN_SPACE_BETWEEN, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_t* hl = box(head, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(hl, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_column(hl, 8, 0);
    label(hl, Ico::VIDEO, I24, WHITE);
    label(hl, "Esterna", S22, WHITE);

    lv_obj_t* live = box(head, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(live, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_column(live, 4, 0);
    lv_obj_set_style_bg_color(live, c(0x3A352F), 0);
    lv_obj_set_style_bg_opa(live, LV_OPA_COVER, 0);
    lv_obj_set_style_radius(live, 14, 0);
    lv_obj_set_style_pad_hor(live, 10, 0);
    label(live, Ico::POINT, I24, LIVE_DOT);
    label(live, "Live", S22, WHITE);

    lv_obj_t* area = lv_obj_create(t);
    lv_obj_remove_style_all(area);
    lv_obj_set_width(area, LV_PCT(100));
    lv_obj_set_flex_grow(area, 1);
    lv_obj_set_style_radius(area, 16, 0);
    lv_obj_set_style_clip_corner(area, true, 0);
    lv_obj_set_style_bg_color(area, c(0x3A352F), 0);
    lv_obj_set_style_bg_opa(area, LV_OPA_COVER, 0);
    lv_obj_set_scrollable(area, false);

    lv_obj_t* ph = label(area, Ico::CAMERA, I72, 0x6E675D);    // segnaposto finche' non arriva il video
    lv_obj_center(ph);

    lv_obj_update_layout(t);   // serve per conoscere la dimensione reale del riquadro
    cam.create(area, 0, 0, lv_obj_get_width(area), lv_obj_get_height(area), entityId);
}


// ============================================================================
//  LUCI
// ============================================================================
namespace
{
    struct LightDef { const char* entity; const char* domain; const char* name; };

    const LightDef kLights[LightsPanel::COUNT] = {
        { "switch.led_cucina_switch_0",          "switch", "Cucina"    },
        { "switch.penisola_switch_0",            "switch", "Penisola"  },
        { "switch.deviatore_wi_fi_ble_switch_1", "switch", "Soggiorno" },
        { "switch.luce_dispensa_switch_0",       "switch", "Dispensa"  },
        { "switch.luce_armadio_switch_0",        "switch", "Armadio"   },
        { "switch.lampada_presa",                "switch", "Lampada"   },
        { "switch.luce_garage_switch_0",         "switch", "Garage"    },
        { "switch.luce_scala_switch_0",          "switch", "Scala"     },
        { "light.cuoricini",                     "light",  "Cuoricini" },
    };
}

void LightsPanel::create(lv_obj_t* cell)
{
    static const int32_t cols[] = { LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_TEMPLATE_LAST };
    static const int32_t rows[] = { LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_TEMPLATE_LAST };

    lv_obj_t* cd = card(cell);
    lv_obj_set_flex_flow(cd, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_pad_ver(cd, 16, 0);

    lv_obj_t* head = box(cd, LV_FLEX_FLOW_ROW, LV_PCT(100));
    lv_obj_set_flex_align(head, LV_FLEX_ALIGN_SPACE_BETWEEN, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_bottom(head, 10, 0);
    lv_obj_t* hl = box(head, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(hl, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_column(hl, 10, 0);
    label(hl, Ico::BULB, I24, TEXT);
    label(hl, "Luci principali", B24, TEXT);
    countL = label(head, "", S22, MUTED);

    lv_obj_t* g = lv_obj_create(cd);
    lv_obj_remove_style_all(g);
    lv_obj_set_size(g, LV_PCT(100), 0);
    lv_obj_set_flex_grow(g, 1);                 // i chip occupano tutta l'altezza della card
    lv_obj_set_grid_dsc_array(g, cols, rows);
    lv_obj_set_style_pad_gap(g, 12, 0);
    lv_obj_set_scrollable(g, false);

    for(int i = 0; i < COUNT; i++)
    {
        repo().watch(kLights[i].entity);

        Chip& ch = chips[i];
        ch.self = this;
        ch.index = i;

        ch.obj = lv_obj_create(g);
        lv_obj_set_grid_cell(ch.obj, LV_GRID_ALIGN_STRETCH, i % 3, 1, LV_GRID_ALIGN_STRETCH, i / 3, 1);
        lv_obj_set_flex_flow(ch.obj, LV_FLEX_FLOW_ROW);
        lv_obj_set_flex_align(ch.obj, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
        lv_obj_set_style_radius(ch.obj, 16, 0);
        lv_obj_set_style_pad_hor(ch.obj, 14, 0);
        lv_obj_set_style_pad_column(ch.obj, 8, 0);
        lv_obj_set_style_border_width(ch.obj, 2, 0);
        lv_obj_set_style_bg_opa(ch.obj, LV_OPA_COVER, 0);
        lv_obj_set_scrollable(ch.obj, false);

        ch.icon = label(ch.obj, Ico::BULB, I24);
        ch.name = label(ch.obj, kLights[i].name, S22);

        lv_obj_add_event_cb(ch.obj, onChip, LV_EVENT_CLICKED, &ch);
    }

    refresh();
    autoRefresh(2000);
}

void LightsPanel::refresh()
{
    int on = 0;

    for(auto& ch : chips)
    {
        bool isOn = repo().getBoolState(kLights[ch.index].entity);
        on += isOn ? 1 : 0;

        lv_obj_set_style_bg_color(ch.obj, c(isOn ? OLIVE : BG), 0);
        lv_obj_set_style_border_color(ch.obj, c(isOn ? OLIVE : CARD_LINE), 0);
        lv_obj_set_style_text_color(ch.icon, c(isOn ? WHITE : MUTED), 0);
        lv_obj_set_style_text_color(ch.name, c(isOn ? WHITE : MUTED), 0);
    }

    setf(countL, "%d/%d accese", on, COUNT);
}

void LightsPanel::onChip(lv_event_t* e)
{
    Chip* ch = static_cast<Chip*>(lv_event_get_user_data(e));
    bool isOn = repo().getBoolState(kLights[ch->index].entity);
    repo().requestServiceCall(kLights[ch->index].domain, isOn ? "turn_off" : "turn_on",
                              kLights[ch->index].entity, isOn ? "off" : "on");
    EventLog::instance().add(std::string("Luce ") + kLights[ch->index].name + (isOn ? " spenta" : " accesa"));
    ch->self->refresh();
}


// ============================================================================
//  CLIMA E IMPIANTI (colori come StatusCardsRow)
// ============================================================================
void ClimatePanel::create(lv_obj_t* cell)
{
    lv_obj_t* cd = card(cell);
    lv_obj_set_flex_flow(cd, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_pad_ver(cd, 14, 0);
    headRow(cd, Ico::TEMP, "Clima e impianti", 8);

    lv_obj_t* row = box(cd, LV_FLEX_FLOW_ROW, LV_PCT(100));
    lv_obj_set_flex_grow(row, 1);
    lv_obj_set_style_pad_column(row, 14, 0);

    makeM(row, condensa, nullptr, "Limite condensa", R36, S20);
    makeM(row, acs,      nullptr, "ACS",             R36, S20);
    makeM(row, impianto, nullptr, "Impianto",        R36, S20);

    for(const char* id : { "sensor.ur_limite_anticondensa", "sensor.shellyem_c45bbee1d2cb_channel_2_power",
                           "sensor.acs_pdc", "sensor.acs_casa", "sensor.mandata_pdc", "sensor.ritorno_casa" })
        repo().watch(id);

    refresh();
    autoRefresh(3000);
}

void ClimatePanel::refresh()
{
    float cond = repo().getFloatState("sensor.ur_limite_anticondensa");
    float pump = repo().getFloatState("sensor.shellyem_c45bbee1d2cb_channel_2_power");
    setf(condensa.value, "%.1f %%", cond);
    lv_label_set_text(condensa.sub, "");
    paintM(condensa, pump > 150.0f ? Tone{ RED, false } : Tone{ OLIVE, false });

    float acsPdc  = repo().getFloatState("sensor.acs_pdc");
    float acsCasa = repo().getFloatState("sensor.acs_casa");
    setf(acs.value, "%.1f °C", acsPdc);
    setf(acs.sub, "%.1f °C casa", acsCasa);
    paintM(acs, acsPdc >= 34.0f ? Tone{ OLIVE, false } : Tone{ RED, false });

    float mandata = repo().getFloatState("sensor.mandata_pdc");
    float ritorno = repo().getFloatState("sensor.ritorno_casa");
    setf(impianto.value, "%.1f °C", mandata);
    setf(impianto.sub, "%.1f °C casa", ritorno);
    paintM(impianto, Tone{ SAND, false });
}


// ============================================================================
//  STATO SISTEMA: allarme + auto
// ============================================================================
void SystemPanel::create(lv_obj_t* cell)
{
    lv_obj_t* cd = card(cell);
    lv_obj_set_flex_flow(cd, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_pad_ver(cd, 14, 0);
    headRow(cd, Ico::SHIELD, "Stato sistema", 8);

    lv_obj_t* row = box(cd, LV_FLEX_FLOW_ROW, LV_PCT(100));
    lv_obj_set_flex_grow(row, 1);
    lv_obj_set_style_pad_column(row, 12, 0);

    makeM(row, alarm, nullptr, "Allarme", R36, S20);
    makeM(row, car,   nullptr, "Auto",    R36, S18);
    lv_label_set_long_mode(car.sub, LV_LABEL_LONG_WRAP);
    lv_obj_set_width(car.sub, LV_PCT(100));

    repo().watch("alarm_control_panel.80_000_zone_1_alarm");
    repo().watch("sensor.saic_mg_mg4_soc");
    repo().watch("sensor.ultima_sessione_energia");
    repo().watch("sensor.ultima_sessione_timestamp");

    refresh();
    autoRefresh(3000);
}

void SystemPanel::refresh()
{
    std::string s = repo().getState("alarm_control_panel.80_000_zone_1_alarm");
    Tone t{ SAND, false };
    const char* mode = "Sconosciuto";
    const char* state = "";

    if(s == "armed_night")      { mode = "Notte";       state = "Inserito"; t = { MUSTARD, true };  }
    else if(s == "armed_away")  { mode = "Completo";    state = "Inserito"; t = { RED, false };     }
    else if(s == "disarmed")    { mode = "Disinserito"; state = "";         t = { OLIVE, false };   }

    lv_label_set_text(alarm.value, mode);
    lv_label_set_text(alarm.sub, state);
    paintM(alarm, t);

    float soc    = repo().getFloatState("sensor.saic_mg_mg4_soc");
    float energy = repo().getFloatState("sensor.ultima_sessione_energia");
    setf(car.value, "SOC %d%%", (int)soc);
    setf(car.sub, "%.0f kWh\n%s", energy,
         fmtDateTime(repo().getState("sensor.ultima_sessione_timestamp")).c_str());
    paintM(car, Tone{ SAND, false });
}


// ============================================================================
//  TEMPERATURE INTERNE: giorno / notte (sensori come DayNightCardsRow)
// ============================================================================
void RoomsPanel::create(lv_obj_t* cell)
{
    lv_obj_t* cd = card(cell);
    lv_obj_set_flex_flow(cd, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_pad_ver(cd, 14, 0);
    headRow(cd, Ico::HOME2, "Temperature interne", 8);

    lv_obj_t* row = box(cd, LV_FLEX_FLOW_ROW, LV_PCT(100));
    lv_obj_set_flex_grow(row, 1);
    lv_obj_set_style_pad_column(row, 12, 0);

    struct Def { const char* icon; const char* title; Tone tone; lv_obj_t** t; lv_obj_t** h; };
    Def defs[2] = {
        { Ico::SUN,  "Giorno", { MUSTARD_D, false },  &dayT,   &dayH   },
        { Ico::MOON, "Notte",  { NIGHT,     false }, &nightT, &nightH },
    };

    for(const Def& d : defs)
    {
        struct { lv_obj_t* obj = nullptr; lv_obj_t* icon = nullptr; lv_obj_t* title = nullptr;
                 lv_obj_t* value = nullptr; lv_obj_t* sub = nullptr; } m;
        makeM(row, m, d.icon, d.title, R36, S20);
        paintM(m, d.tone);
        *d.t = m.value;
        *d.h = m.sub;
    }

    for(const char* id : { "sensor.soggiorno_soggiorno_temperatura_soggiorno", "sensor.soggiorno_soggiorno_umidita_soggiorno",
                           "sensor.zigbee_notte_temperature", "sensor.zigbee_notte_humidity" })
        repo().watch(id);

    refresh();
    autoRefresh(5000);
}

void RoomsPanel::refresh()
{
    setf(dayT,   "%.1f °C", repo().getFloatState("sensor.soggiorno_soggiorno_temperatura_soggiorno"));
    setf(dayH,   "%.1f %%", repo().getFloatState("sensor.soggiorno_soggiorno_umidita_soggiorno"));
    setf(nightT, "%.1f °C", repo().getFloatState("sensor.zigbee_notte_temperature"));
    setf(nightH, "%.1f %%", repo().getFloatState("sensor.zigbee_notte_humidity"));
}


// ============================================================================
//  METEO LOCALE / QUALITA' ARIA / STAZIONE (colori come WeatherCardsRow)
// ============================================================================
namespace
{
    // Tessera "titolo con icona / valore serif / sottotitolo"
    void buildInfoTile(lv_obj_t* cell, lv_obj_t*& obj, lv_obj_t*& icon, lv_obj_t*& title,
                       lv_obj_t*& value, lv_obj_t*& sub, const char* ic, const char* name, const lv_font_t* vf)
    {
        obj = tile(cell, SAND);
        lv_obj_set_flex_flow(obj, LV_FLEX_FLOW_COLUMN);
        lv_obj_set_flex_align(obj, LV_FLEX_ALIGN_SPACE_BETWEEN, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START);
        lv_obj_set_style_pad_ver(obj, 14, 0);
        lv_obj_set_style_pad_hor(obj, 20, 0);

        lv_obj_t* head = box(obj, LV_FLEX_FLOW_ROW, LV_PCT(100));
        lv_obj_set_flex_align(head, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
        lv_obj_set_style_pad_column(head, 8, 0);
        icon  = label(head, ic, I24);
        title = label(head, name, B22);
        value = label(obj, "--", vf);
        sub   = label(obj, "", S20);
    }

    void paintInfo(lv_obj_t* obj, lv_obj_t* icon, lv_obj_t* title, lv_obj_t* value, lv_obj_t* sub, Tone t)
    {
        tint(obj, t);
        ink(icon, t); ink(title, t); ink(value, t); ink(sub, t, true);
    }
}

void WeatherTile::create(lv_obj_t* cell)
{
    buildInfoTile(cell, obj, icon, title, value, sub, Ico::CLOUD, "Temperatura ingresso", R44);
    repo().watch("sensor.temp_ingresso");
    repo().watch("sensor.openweathermap_weather");
    repo().watch("sensor.openweathermap_humidity");
    refresh();
    autoRefresh(5000);
}

void WeatherTile::refresh()
{
    float temp = repo().getFloatState("sensor.temp_ingresso");
    setf(value, "%.1f °C", temp);
    std::string cond = repo().getState("sensor.openweathermap_weather");
    setf(sub, "%s · umidità %.0f%%", cond.empty() ? "--" : cond.c_str(),
         repo().getFloatState("sensor.openweathermap_humidity"));
    paintInfo(obj, icon, title, value, sub, toneMeteo(temp));
}

void AirTile::create(lv_obj_t* cell)
{
    buildInfoTile(cell, obj, icon, title, value, sub, Ico::LEAF, "Qualità aria", R44);
    repo().watch("sensor.aqi_peggiore");
    repo().watch("sensor.inquinante_dominante_peggiore");
    refresh();
    autoRefresh(5000);
}

void AirTile::refresh()
{
    float aqi = repo().getFloatState("sensor.aqi_peggiore");
    setf(value, "AQI %d", (int)aqi);
    setf(sub, "%s · %s", pollutantLabel(repo().getState("sensor.inquinante_dominante_peggiore")).c_str(), aqiText(aqi));
    paintInfo(obj, icon, title, value, sub, toneAqi(aqi));
}

void StationTile::create(lv_obj_t* cell)
{
    buildInfoTile(cell, obj, icon, title, value, sub, Ico::PIN, "Stazione", R36);
    repo().watch("sensor.aqi_peggiore");
    repo().watch("sensor.stazione_aqi_peggiore");
    repo().watch("sensor.via_meduna_pm2_5");
    repo().watch("sensor.via_dei_cipressi_pm2_5");
    refresh();
    autoRefresh(5000);
}

void StationTile::refresh()
{
    std::string name = repo().getState("sensor.stazione_aqi_peggiore");
    lv_label_set_text(sub, name.empty() ? "--" : name.c_str());

    if(name == "Via Meduna")            setf(value, "%.1f µg/mc", repo().getFloatState("sensor.via_meduna_pm2_5"));
    else if(name == "Via dei Cipressi") setf(value, "%.1f µg/mc", repo().getFloatState("sensor.via_dei_cipressi_pm2_5"));
    else                                lv_label_set_text(value, "-- µg/mc");

    paintInfo(obj, icon, title, value, sub, toneAqi(repo().getFloatState("sensor.aqi_peggiore")));
}


// ============================================================================
//  IMPEGNI (CalendarFeed, "OGGI" come nella v1.3.2)
// ============================================================================
void AgendaPanel::create(lv_obj_t* cell)
{
    lv_obj_t* cd = card(cell);
    lv_obj_set_flex_flow(cd, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_pad_ver(cd, 14, 0);
    headRow(cd, Ico::CALENDAR, "Impegni", 4);

    list = box(cd, LV_FLEX_FLOW_COLUMN, LV_PCT(100));
    lv_obj_set_flex_grow(list, 1);

    auto& feed = CalendarFeed::instance();
    feed.watchCalendar("calendar.omar_rossoni_gmail_com");
    feed.watchCalendar("calendar.sendina");
    feed.start();

    refresh();
    autoRefresh(30000);
}

void AgendaPanel::refresh()
{
    lv_obj_clean(list);

    auto events = CalendarFeed::instance().getEvents();

    if(events.empty())
    {
        label(list, "Nessun impegno nei prossimi 7 giorni", S26, MUTED);
        return;
    }

    int n = 0;
    for(const auto& ev : events)
    {
        if(n >= 5) break;

        uint32_t color = calendarColor(ev.calendarId);

        lv_obj_t* r = box(list, LV_FLEX_FLOW_ROW, LV_PCT(100));
        lv_obj_set_flex_align(r, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
        lv_obj_set_style_pad_column(r, 12, 0);
        lv_obj_set_style_pad_ver(r, 5, 0);
        if(n > 0)
        {
            lv_obj_set_style_border_side(r, LV_BORDER_SIDE_TOP, 0);
            lv_obj_set_style_border_width(r, 2, 0);
            lv_obj_set_style_border_color(r, c(LINE), 0);
        }

        lv_obj_t* d = label(r, isToday(ev) ? "OGGI" : ev.displayDate.c_str(), B24, color);
        lv_obj_set_width(d, 96);
        lv_obj_t* t = label(r, ev.displayTime.c_str(), S24, MUTED);
        lv_obj_set_width(t, 210);
        lv_obj_t* s = label(r, ev.summary.c_str(), S26, TEXT);
        lv_label_set_long_mode(s, LV_LABEL_LONG_DOT);
        lv_obj_set_height(s, lv_font_get_line_height(S26));
        lv_obj_set_flex_grow(s, 1);

        n++;
    }
}


// ============================================================================
//  SCALDA BAGNETTO (riga sottile: stato scaldino + abilitazione)
// ============================================================================
void HeaterBar::create(lv_obj_t* cell)
{
    chips[0].domain = "switch";
    chips[0].entity = "switch.scalda_bagnetto";
    chips[0].name   = "Scalda bagnetto";
    chips[0].icon   = Ico::TEMP;
    chips[0].onColor = ORANGE;

    chips[1].domain = "input_boolean";
    chips[1].entity = "input_boolean.scalda_bagnetto_abilitato";
    chips[1].name   = "Abilitato";
    chips[1].icon   = Ico::CALENDAR;
    chips[1].onColor = OLIVE;

    static const int32_t cols[] = { LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_TEMPLATE_LAST };
    static const int32_t rows[] = { LV_GRID_FR(1), LV_GRID_TEMPLATE_LAST };

    lv_obj_t* g = lv_obj_create(cell);
    lv_obj_remove_style_all(g);
    lv_obj_set_size(g, LV_PCT(100), LV_PCT(100));
    lv_obj_set_grid_dsc_array(g, cols, rows);
    lv_obj_set_style_pad_gap(g, 8, 0);
    lv_obj_set_scrollable(g, false);

    for(int i = 0; i < 2; i++)
    {
        Chip& ch = chips[i];
        repo().watch(ch.entity);

        ch.obj = lv_obj_create(g);
        lv_obj_set_grid_cell(ch.obj, LV_GRID_ALIGN_STRETCH, i, 1, LV_GRID_ALIGN_STRETCH, 0, 1);
        lv_obj_set_flex_flow(ch.obj, LV_FLEX_FLOW_ROW);
        lv_obj_set_flex_align(ch.obj, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
        lv_obj_set_style_radius(ch.obj, 16, 0);
        lv_obj_set_style_pad_hor(ch.obj, 20, 0);
        lv_obj_set_style_pad_ver(ch.obj, 0, 0);
        lv_obj_set_style_pad_column(ch.obj, 12, 0);
        lv_obj_set_style_border_width(ch.obj, 2, 0);
        lv_obj_set_style_bg_opa(ch.obj, LV_OPA_COVER, 0);
        lv_obj_set_scrollable(ch.obj, false);

        ch.iconL  = label(ch.obj, ch.icon, I24);
        ch.nameL  = label(ch.obj, ch.name, S22);
        lv_obj_set_flex_grow(ch.nameL, 1);
        ch.stateL = label(ch.obj, "", B22);

        lv_obj_add_event_cb(ch.obj, onChip, LV_EVENT_CLICKED, &ch);
    }

    refresh();
    autoRefresh(2000);
}

void HeaterBar::refresh()
{
    for(auto& ch : chips)
    {
        bool isOn = repo().getBoolState(ch.entity);

        lv_obj_set_style_bg_color(ch.obj, c(isOn ? ch.onColor : BG), 0);
        lv_obj_set_style_border_color(ch.obj, c(isOn ? ch.onColor : CARD_LINE), 0);
        lv_obj_set_style_text_color(ch.iconL,  c(isOn ? WHITE : MUTED), 0);
        lv_obj_set_style_text_color(ch.nameL,  c(isOn ? WHITE : MUTED), 0);
        lv_obj_set_style_text_color(ch.stateL, c(isOn ? WHITE : MUTED), 0);
        lv_label_set_text(ch.stateL, isOn ? "ON" : "OFF");
    }
}

void HeaterBar::onChip(lv_event_t* e)
{
    Chip* ch = static_cast<Chip*>(lv_event_get_user_data(e));
    bool isOn = repo().getBoolState(ch->entity);

    repo().requestServiceCall(ch->domain, isOn ? "turn_off" : "turn_on",
                              ch->entity, isOn ? "off" : "on");
    EventLog::instance().add(std::string(ch->name) + (isOn ? " disattivato" : " attivato"));

    // aggiorna subito i colori (la cache e' gia' ottimistica)
    bool nowOn = !isOn;
    lv_obj_set_style_bg_color(ch->obj, c(nowOn ? ch->onColor : BG), 0);
    lv_obj_set_style_border_color(ch->obj, c(nowOn ? ch->onColor : CARD_LINE), 0);
    lv_obj_set_style_text_color(ch->iconL,  c(nowOn ? WHITE : MUTED), 0);
    lv_obj_set_style_text_color(ch->nameL,  c(nowOn ? WHITE : MUTED), 0);
    lv_obj_set_style_text_color(ch->stateL, c(nowOn ? WHITE : MUTED), 0);
    lv_label_set_text(ch->stateL, nowOn ? "ON" : "OFF");
}
