#pragma once

#include "../widgets/PanelWidgets.h"
#include "../widgets/SystemInfoPopup.h"

// Layout verticale 1080x1920 (altezze in pixel, ultima riga flessibile):
//
//  [ allerta meteo                          ]  80
//  [ orologio / data            meteo       ] 130
//  [ Casa        | Fotovoltaico             ] 170
//  [ Batteria    | Rete                     ] 170
//  [ Telecamera | Luci principali           ] 300
//  [ Clima e impianti (3 tessere)           ] 190
//  [ Stato sistema | Temperature interne    ] 200
//  [ Meteo | Qualita' aria | Stazione       ] 130
//  [ Impegni                                ] resto
//  [ Scalda bagnetto | Abilitato            ]  60
class HomeScreen
{

public:

    void create();


private:

    AlertBanner alert;
    ClockBar clockBar;

    EnergyTile houseTile;
    EnergyTile pvTile;
    EnergyTile batteryTile;
    EnergyTile gridTile;

    CameraPanel cameraPanel;
    LightsPanel lightsPanel;

    ClimatePanel climatePanel;
    SystemPanel systemPanel;
    RoomsPanel roomsPanel;

    WeatherTile weatherTile;
    AirTile airTile;
    StationTile stationTile;

    AgendaPanel agendaPanel;
    HeaterBar heaterBar;
    SystemInfoPopup infoPopup;
};
