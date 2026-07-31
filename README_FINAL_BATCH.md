# Batch di aggiornamenti: layout card, calendario, auto, LED movimento

## File da SOSTITUIRE per intero
- `src/ui/widgets/StatusCardsRow.cpp`  (Limite condensa su 2 righe)
- `src/ui/widgets/EnergyGaugesPanel.cpp`  (testo energia Rete più leggibile)

## File NUOVI da aggiungere
- `src/ui/widgets/CarStatusCard.h`
- `src/ui/widgets/CarStatusCard.cpp`
- `src/ui/widgets/MotionLedsRow.h`
- `src/ui/widgets/MotionLedsRow.cpp`

## File da RIATTIVARE (già esistenti, sospesi in precedenza)
- `src/ui/widgets/CalendarCard.cpp`: ripristina la riga
  `updateTimer = lv_timer_create(timerCallback, 30000, this);`
  che avevi commentato. Il blocco precedente era quasi certamente
  causato dal limite di memoria fisso di LVGL (LV_MEM_SIZE 64KB), già
  risolto passando a LV_USE_STDLIB_MALLOC = LV_STDLIB_CLIB — quindi
  ora dovrebbe funzionare normalmente.

## CMakeLists.txt
Aggiungi:
```cmake
src/ui/widgets/CarStatusCard.cpp

src/ui/widgets/MotionLedsRow.cpp
```
(CalendarCard.cpp e CalendarFeed.cpp dovrebbero già esserci da prima)

## HomeScreen.h — aggiungi (non sostituire il file)

```cpp
#include "../widgets/CarStatusCard.h"
#include "../widgets/MotionLedsRow.h"
#include "../widgets/CalendarCard.h"   // se non già presente
```

```cpp
CarStatusCard carStatusCard;
MotionLedsRow motionLeds;
CalendarCard calendarCard;   // se non già presente
```

## HomeScreen.cpp — aggiungi (non sostituire il file)

### 1. LED movimento
Tra la riga con il ClockCard/CameraCard e la riga dei toggle luci —
**verifica tu la posizione esatta**, dato che in questo momento
EnergyGaugesPanel occupa già quello spazio nel layout attuale.
Puoi metterli sopra EnergyGaugesPanel (es. y=300) o in una striscia
dedicata spostando leggermente giù EnergyGaugesPanel per fargli
spazio:

```cpp
motionLeds.create(
    lv_screen_active(),
    27,
    300,   // <-- verifica/aggiusta
    {
        { "binary_sensor.ezviz_esterna_movimento_2", "Esterna" },
        { "binary_sensor.ezviz_terrazzo_movimento",  "Terrazzo" },
        { "binary_sensor.cam_garage_movimento_2",    "Garage" },
    }
);
```

### 2. Card Auto (dopo Giorno/Notte)
Subito dopo la chiamata a `dayNightRow.create(...)`:
```cpp
carStatusCard.create(
    lv_screen_active(),
    0,
    <Y>,     // <-- sotto dayNightRow, verifica tu la posizione esatta
    1080,
    180      // altezza indicativa
);
```

### 3. Card Calendario (in fondo)
Per ultima, dopo tutto il resto:
```cpp
calendarCard.create(
    lv_screen_active(),
    0,
    <Y>,     // <-- sotto tutto il resto, verifica tu la posizione
    1080,
    220
);
```

Come sempre, non ho scelto io i valori `<Y>` mancanti: le tue
coordinate reali sono cambiate molte volte durante lo sviluppo, e
preferisco che tu verifichi a schermo prima di fissare un numero,
invece di rischiare ancora sovrapposizioni indovinate a caso.

## Riepilogo modifiche

### StatusCardsRow — Limite condensa
Titolo ora "Limite\ncondensa" su 2 righe, centrato; il valore
sottostante è stato spostato più in basso (offset 84 invece di 44)
per non sovrapporsi.

### EnergyGaugesPanel — Rete
Il testo con l'energia prelevata/immessa oggi ora:
- usa un font più piccolo (montserrat_14 invece di 20)
- è vincolato in larghezza e va a capo su 2 righe se necessario
  ("Prelevata X" / "Immessa Y kWh")
- è centrato orizzontalmente
- la card è stata alzata di ~24px in altezza per fare spazio

### CarStatusCard (nuova)
Mostra SOC dell'auto (sensor.saic_mg_mg4_soc), energia dell'ultima
sessione di ricarica (sensor.ultima_sessione_energia) e data/ora
(sensor.ultima_sessione_timestamp, formattata come "dd/mm HH:MM" se
il formato è ISO 8601 standard — altrimenti mostra la stringa grezza
così com'è).

### MotionLedsRow (nuova)
3 LED nativi LVGL (lv_led), rossi quando il relativo binary_sensor è
"on" (movimento rilevato), spenti/tenui altrimenti. Ogni LED ha
un'etichetta sopra con il nome della telecamera.

### CalendarCard
Timer di refresh ripristinato (30s), probabilmente il vecchio bug
di blocco è già risolto dal fix della memoria LVGL.
