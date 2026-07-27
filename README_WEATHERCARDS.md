# Nuova funzionalità: 4 card meteo/qualità aria/allerta/stazione

## File da SOSTITUIRE per intero
- `src/services/HomeAssistantClient.h`
- `src/services/HomeAssistantClient.cpp`
- `src/services/HomeAssistantRepository.h`
- `src/services/HomeAssistantRepository.cpp`

## File NUOVI da aggiungere
- `src/ui/widgets/WeatherCardsRow.h`
- `src/ui/widgets/WeatherCardsRow.cpp`

## Cosa è cambiato nel repository (importante)
Prima, la cache conservava solo lo `state` grezzo di ogni entità.
Ora conserva il JSON COMPLETO (stato + attributi), perché la card
"Allerta meteo" ha bisogno di leggere attributi come `expires`,
`event`, `awareness_level` — non solo lo stato on/off dell'entità.

Ho aggiunto un nuovo metodo:
```cpp
std::string getAttribute(const std::string& entityId, const std::string& attributeName) const;
```

Tutti i metodi esistenti (`getState`, `getFloatState`, `getBoolState`,
`requestServiceCall`, `watch`) funzionano ESATTAMENTE come prima dal
punto di vista di chi li chiama (EnergyCard, EntityButtonRow,
StatusCardsRow, ClockCard, ecc.) — non serve modificare nessun altro
file oltre a quelli elencati sopra.

## Come integrare WeatherCardsRow in HomeScreen

In `HomeScreen.h`, aggiungi:
```cpp
#include "../widgets/WeatherCardsRow.h"
...
WeatherCardsRow weatherRow;
```

In `HomeScreen.cpp`, dentro `create()`, aggiungi una chiamata simile a:
```cpp
weatherRow.create(
    lv_screen_active(),
    0,
    <Y>,     // <-- calcola in base alla posizione dell'ultimo elemento
             //     che hai attualmente (probabilmente subito sotto
             //     statusRow, dato quanto discusso finora)
    1080,
    180
);
```

Non ho preso io questa decisione sul valore esatto di `<Y>` perché le
coordinate del tuo layout sono cambiate diverse volte e preferisco
che tu verifichi a schermo la posizione esatta dell'ultimo elemento
(statusRow) prima di aggiungere questa riga, per evitare sovrapposizioni
come già successo in precedenza.

## Entità Home Assistant usate
- sensor.openweathermap_temperature
- sensor.openweathermap_weather
- binary_sensor.meteoalarm_friuli_venezia_giulia (+ attributi: event, expires, awareness_level)
- sensor.aqi_peggiore
- sensor.inquinante_dominante_peggiore
- sensor.stazione_aqi_peggiore
- sensor.via_meduna_pm2_5
- sensor.via_dei_cipressi_pm2_5

## Limitazioni note
- **Orario "Fino alle HH:MM"**: estratto direttamente dalla stringa ISO
  restituita da Home Assistant (es. "2026-07-27T18:30:00+00:00" -> "18:30"),
  SENZA conversione al fuso orario locale del dispositivo. Il dashboard
  originale in JavaScript faceva la conversione automaticamente
  (`new Date(exp).getHours()`); qui, per semplicità, mostriamo l'orario
  così come riportato — se l'offset UTC di HA differisce dal fuso del
  Raspberry Pi, l'ora mostrata potrebbe non corrispondere all'ora locale.
  Se vuoi la conversione esatta, si può aggiungere in un secondo momento
  (serve parsare anche l'offset e fare un calcolo).
- Nessuna icona (stesso discorso già fatto per StatusCardsRow): solo
  testo e colori di sfondo.
- I colori di sfondo usano `LV_OPA_20/30/40` (trasparenza) sul colore
  scelto, sovrapposto al nero della card sottostante — leggermente
  diverso dagli `rgba(...)` esatti del CSS originale, ma stessa logica
  di soglie/colori.

## CMakeLists.txt
Aggiungi:
```cmake
src/ui/widgets/WeatherCardsRow.cpp
```
(vicino alle altre righe `src/ui/widgets/...`)
