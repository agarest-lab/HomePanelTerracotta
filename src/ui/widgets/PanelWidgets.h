#pragma once

#include <lvgl.h>
#include <array>
#include <string>

#include "CameraCard.h"
#include "WeatherIconCard.h"   // solo per le dichiarazioni delle icone *_96

// Pannelli del layout verticale 1080x1920. Ogni pannello si crea dentro una
// cella della griglia (create(cell)) e si aggiorna da solo con un timer LVGL,
// leggendo dalla cache di HomeAssistantRepository (nessun I/O di rete).

class Panel
{
public:
    virtual ~Panel() = default;
    virtual void refresh() = 0;

protected:
    void autoRefresh(uint32_t ms);
};


// Striscia allerta meteo (colore per livello meteoalarm)
class AlertBanner : public Panel
{
public:
    void create(lv_obj_t* cell);
    void refresh() override;

private:
    lv_obj_t* obj = nullptr;
    lv_obj_t* icon = nullptr;
    lv_obj_t* title = nullptr;
    lv_obj_t* event = nullptr;
    lv_obj_t* info = nullptr;
};


// Orologio, data e meteo attuale
class ClockBar : public Panel
{
public:
    void create(lv_obj_t* cell);
    void refresh() override;

private:
    lv_obj_t* timeL = nullptr;
    lv_obj_t* dateL = nullptr;
    lv_obj_t* icon = nullptr;
    lv_obj_t* tempL = nullptr;
    lv_obj_t* condL = nullptr;
    const lv_img_dsc_t* shownIcon = nullptr;
};


// Tessera energia (Consumo casa / Fotovoltaico / Batteria / Rete)
class EnergyTile : public Panel
{
public:
    enum class Kind { House, Pv, Battery, Grid };

    void create(lv_obj_t* cell, Kind kind);
    void refresh() override;

private:
    Kind kind = Kind::House;
    lv_obj_t* obj = nullptr;
    lv_obj_t* iconL = nullptr;
    lv_obj_t* titleL = nullptr;
    lv_obj_t* valueL = nullptr;
    lv_obj_t* bar = nullptr;
    lv_obj_t* subL = nullptr;
    lv_obj_t* subR = nullptr;

    void apply(float barPct, bool light, uint32_t bg);
};


class CameraPanel
{
public:
    void create(lv_obj_t* cell, const std::string& entityId);

private:
    CameraCard cam;
};


class LightsPanel : public Panel
{
public:
    static constexpr int COUNT = 9;

    void create(lv_obj_t* cell);
    void refresh() override;

private:
    struct Chip
    {
        LightsPanel* self = nullptr;
        int index = 0;
        lv_obj_t* obj = nullptr;
        lv_obj_t* icon = nullptr;
        lv_obj_t* name = nullptr;
    };

    std::array<Chip, COUNT> chips;
    lv_obj_t* countL = nullptr;

    static void onChip(lv_event_t* e);
};


class ClimatePanel : public Panel
{
public:
    void create(lv_obj_t* cell);
    void refresh() override;

private:
    struct M { lv_obj_t* obj = nullptr; lv_obj_t* icon = nullptr; lv_obj_t* title = nullptr; lv_obj_t* value = nullptr; lv_obj_t* sub = nullptr; };
    M condensa, acs, impianto;
};


class SystemPanel : public Panel
{
public:
    void create(lv_obj_t* cell);
    void refresh() override;

private:
    struct M { lv_obj_t* obj = nullptr; lv_obj_t* icon = nullptr; lv_obj_t* title = nullptr; lv_obj_t* value = nullptr; lv_obj_t* sub = nullptr; };
    M alarm, car;
};


// Temperature interne: giorno e notte
class RoomsPanel : public Panel
{
public:
    void create(lv_obj_t* cell);
    void refresh() override;

private:
    lv_obj_t* dayT = nullptr;   lv_obj_t* dayH = nullptr;
    lv_obj_t* nightT = nullptr; lv_obj_t* nightH = nullptr;
};


class WeatherTile : public Panel
{
public:
    void create(lv_obj_t* cell);
    void refresh() override;

private:
    lv_obj_t* obj = nullptr; lv_obj_t* icon = nullptr; lv_obj_t* title = nullptr;
    lv_obj_t* value = nullptr; lv_obj_t* sub = nullptr;
};


class AirTile : public Panel
{
public:
    void create(lv_obj_t* cell);
    void refresh() override;

private:
    lv_obj_t* obj = nullptr; lv_obj_t* icon = nullptr; lv_obj_t* title = nullptr;
    lv_obj_t* value = nullptr; lv_obj_t* sub = nullptr;
};


class StationTile : public Panel
{
public:
    void create(lv_obj_t* cell);
    void refresh() override;

private:
    lv_obj_t* obj = nullptr; lv_obj_t* icon = nullptr; lv_obj_t* title = nullptr;
    lv_obj_t* value = nullptr; lv_obj_t* sub = nullptr;
};


class AgendaPanel : public Panel
{
public:
    void create(lv_obj_t* cell);
    void refresh() override;

private:
    lv_obj_t* list = nullptr;
};


// Riga sottile sotto l'agenda: stato di switch.scalda_bagnetto e di
// input_boolean.scalda_bagnetto_abilitato. Ogni chip e' acceso (ON) o spento (OFF)
// e al tocco commuta l'entita'.
class HeaterBar : public Panel
{
public:
    void create(lv_obj_t* cell);
    void refresh() override;

private:
    struct Chip
    {
        const char* domain = nullptr;
        const char* entity = nullptr;
        const char* name = nullptr;
        const char* icon = nullptr;
        uint32_t onColor = 0;
        lv_obj_t* obj = nullptr;
        lv_obj_t* iconL = nullptr;
        lv_obj_t* nameL = nullptr;
        lv_obj_t* stateL = nullptr;
    };

    std::array<Chip, 2> chips;

    static void onChip(lv_event_t* e);
};
