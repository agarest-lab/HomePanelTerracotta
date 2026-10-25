#include "HomeScreen.h"
#include "../Theme.h"

#include <lvgl.h>
#include "../../services/EventLog.h"

namespace
{
    // Spazio minimo tra una card e l'altra (verticale e orizzontale)
    constexpr int GAP = 8;

    // Cella trasparente di una griglia: dentro ci va un pannello
    lv_obj_t* cellOf(lv_obj_t* grid, int col, int colSpan, int row, int rowSpan)
    {
        lv_obj_t* o = lv_obj_create(grid);
        lv_obj_remove_style_all(o);
        lv_obj_set_scrollable(o, false);
        lv_obj_set_grid_cell(o, LV_GRID_ALIGN_STRETCH, col, colSpan,
                                LV_GRID_ALIGN_STRETCH, row, rowSpan);
        return o;
    }

    // Griglia che riempie la cella che la contiene
    lv_obj_t* gridIn(lv_obj_t* parent, const int32_t* cols, const int32_t* rows, int gap)
    {
        lv_obj_t* g = lv_obj_create(parent);
        lv_obj_remove_style_all(g);
        lv_obj_set_size(g, LV_PCT(100), LV_PCT(100));
        lv_obj_set_grid_dsc_array(g, cols, rows);
        lv_obj_set_style_pad_gap(g, gap, 0);
        lv_obj_set_scrollable(g, false);
        return g;
    }
}


void HomeScreen::create()
{
    lv_obj_t* screen = lv_screen_active();

    lv_obj_set_style_bg_color(screen, Theme::c(Theme::BG), 0);
    lv_obj_set_style_bg_opa(screen, LV_OPA_COVER, 0);

    // --- Descrittori griglia (devono restare vivi: static) ---------------
    static const int32_t oneCol[]  = { LV_GRID_FR(1), LV_GRID_TEMPLATE_LAST };
    static const int32_t twoCols[] = { LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_TEMPLATE_LAST };
    static const int32_t camCols[] = { LV_GRID_FR(5), LV_GRID_FR(7), LV_GRID_TEMPLATE_LAST };
    static const int32_t triCols[] = { LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_TEMPLATE_LAST };

    static const int32_t rootRows[]   = { 130, 80, 364, 312, 198, 210, 138, LV_GRID_FR(1), 60, LV_GRID_TEMPLATE_LAST };
    static const int32_t energyRows[] = { 178, 178, LV_GRID_TEMPLATE_LAST };
    static const int32_t oneRow[]     = { LV_GRID_FR(1), LV_GRID_TEMPLATE_LAST };

    // --- Struttura (solo celle vuote) ------------------------------------
    lv_obj_t* root = gridIn(screen, oneCol, rootRows, GAP);
    lv_obj_set_style_pad_all(root, 36, 0);

    lv_obj_t* clockCell  = cellOf(root, 0, 1, 0, 1);
    lv_obj_t* alertCell  = cellOf(root, 0, 1, 1, 1);

    lv_obj_t* energyGrid = gridIn(cellOf(root, 0, 1, 2, 1), twoCols, energyRows, GAP);
    lv_obj_t* houseCell   = cellOf(energyGrid, 0, 1, 0, 1);
    lv_obj_t* pvCell      = cellOf(energyGrid, 1, 1, 0, 1);
    lv_obj_t* batteryCell = cellOf(energyGrid, 0, 1, 1, 1);
    lv_obj_t* gridCell    = cellOf(energyGrid, 1, 1, 1, 1);

    lv_obj_t* camGrid = gridIn(cellOf(root, 0, 1, 3, 1), camCols, oneRow, GAP);
    lv_obj_t* cameraCell = cellOf(camGrid, 0, 1, 0, 1);
    lv_obj_t* lightsCell = cellOf(camGrid, 1, 1, 0, 1);

    lv_obj_t* climateCell = cellOf(root, 0, 1, 4, 1);

    lv_obj_t* sysGrid = gridIn(cellOf(root, 0, 1, 5, 1), twoCols, oneRow, GAP);
    lv_obj_t* systemCell = cellOf(sysGrid, 0, 1, 0, 1);
    lv_obj_t* roomsCell  = cellOf(sysGrid, 1, 1, 0, 1);

    lv_obj_t* infoGrid = gridIn(cellOf(root, 0, 1, 6, 1), triCols, oneRow, GAP);
    lv_obj_t* weatherCell = cellOf(infoGrid, 0, 1, 0, 1);
    lv_obj_t* airCell     = cellOf(infoGrid, 1, 1, 0, 1);
    lv_obj_t* stationCell = cellOf(infoGrid, 2, 1, 0, 1);

    lv_obj_t* agendaCell = cellOf(root, 0, 1, 7, 1);
    lv_obj_t* heaterCell = cellOf(root, 0, 1, 8, 1);

    // Dimensioni reali delle celle prima di creare i pannelli
    // (la telecamera ne ha bisogno per scalare il video)
    lv_obj_update_layout(screen);

    // --- Pannelli ----------------------------------------------------------
    alert.create(alertCell);
    clockBar.create(clockCell);

    houseTile.create(houseCell, EnergyTile::Kind::House);
    pvTile.create(pvCell, EnergyTile::Kind::Pv);
    batteryTile.create(batteryCell, EnergyTile::Kind::Battery);
    gridTile.create(gridCell, EnergyTile::Kind::Grid);

    cameraPanel.create(cameraCell, "camera.ezviz_esterna");
    lightsPanel.create(lightsCell);

    climatePanel.create(climateCell);
    systemPanel.create(systemCell);
    roomsPanel.create(roomsCell);

    weatherTile.create(weatherCell);
    airTile.create(airCell);
    stationTile.create(stationCell);

    agendaPanel.create(agendaCell);
    heaterBar.create(heaterCell);
    
    EventLog::instance().add("Avvio HomePanel");
    infoPopup.create();     // deve stare per ultimo: cosi' la zona di tap e' sopra tutto
}
