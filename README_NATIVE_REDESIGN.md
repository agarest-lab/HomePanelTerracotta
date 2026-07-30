# Redesign con widget nativi LVGL (branch feature/native-widgets-redesign)

## Setup del branch
```bash
cd ~/HomePanel
git add .
git commit -m "WIP: CalendarCard disabilitata (refresh causa rallentamenti, da rivedere)"
git checkout -b feature/native-widgets-redesign
```

## File NUOVI da aggiungere
- `src/ui/widgets/EnergyGaugesPanel.h`
- `src/ui/widgets/EnergyGaugesPanel.cpp`
- `src/ui/widgets/ToggleRow.h`
- `src/ui/widgets/ToggleRow.cpp`

## File da SOSTITUIRE per intero
- `src/ui/screens/HomeScreen.h`
- `src/ui/screens/HomeScreen.cpp`

## File da NON toccare
- StatusCardsRow, WeatherCardsRow: sono già widget nativi (label/colori),
  restano identici.
- ClockCard, CameraCard: restano creati da UI.cpp/UI.h come nel branch
  precedente — non li ho toccati qui, verifica che UI::init() li
  crei ancora correttamente (clock a sinistra, camera a destra, come già
  avevi).
- CalendarCard: NON inclusa in questo HomeScreen (resta in pausa come
  da tua richiesta). Il file esiste ancora nel progetto ma non viene
  usato qui.
- EnergyCard, EntityButtonRow: non più usati da questo HomeScreen, ma
  puoi lasciarli sul disco (non causano errori se non referenziati in
  CMakeLists — anzi, se li togli da CMakeLists risparmi tempo di build).

## CMakeLists.txt
Sostituisci le righe:
```cmake
src/ui/widgets/EnergyCard.cpp
src/ui/widgets/EntityButtonRow.cpp
```
con:
```cmake
src/ui/widgets/EnergyGaugesPanel.cpp
src/ui/widgets/ToggleRow.cpp
```
(le altre righe restano invariate: StatusCardsRow.cpp, WeatherCardsRow.cpp,
ClockCard.cpp, CameraCard.cpp, HomeAssistantClient.cpp,
HomeAssistantRepository.cpp, CameraFeed.cpp, energy_bg.c può restare o
essere tolto dato che non serve più all'immagine di sfondo,
montserrat_orario_140.c resta per l'orologio)

## Layout attuale (coordinate di partenza, DA VERIFICARE a schermo)
- Clock/Camera: y=20, h=280 (invariato, gestito da UI.cpp)
- EnergyGaugesPanel: y=320, 4 gauge in riga, h~345 (arco+titolo+sottotitolo)
- lightsRow (toggle): y=800, h=90
- socketsRow (toggle): y=900, h=90
- statusRow: y=1020, h=180
- weatherRow: y=1210, h=180

Come sempre, queste sono coordinate di partenza ragionevoli ma non
testate sul pannello reale — aspettati di doverle aggiustare guardando
il risultato a schermo, esattamente come per i layout precedenti.

## Novità nei widget
- **EnergyGaugesPanel**: 4 lv_arc (Casa, FV, Batteria, Rete) con valore
  al centro dell'arco e sottotitolo sotto. Colori dinamici per batteria
  (rosso/giallo/verde in base a SOC) e rete (verde=immissione,
  arancione=prelievo).
- **ToggleRow**: sostituisce i bottoni colorati con veri lv_switch.
  Stesso pattern di comunicazione con HomeAssistantRepository
  (watch/getBoolState/requestServiceCall) di EntityButtonRow, quindi
  il comportamento verso Home Assistant è identico — cambia solo
  l'aspetto grafico.

## Nota tecnica sui gauge
Gli archi (lv_arc) sono impostati su range 0-100 internamente e il
valore reale viene convertito in percentuale rispetto a un massimo
scelto (es. 6000W per Casa/FV/Rete, 100% diretto per la Batteria).
Se i tuoi valori reali di casa superano spesso i 6000W, alza il
`maxValue` passato a `setGaugeValue(...)` in
`EnergyGaugesPanel::refresh()` per quel gauge specifico.
