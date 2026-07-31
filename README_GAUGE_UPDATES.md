# Aggiornamenti EnergyGaugesPanel + nuova card Giorno/Notte

## File da SOSTITUIRE per intero
- `src/ui/widgets/EnergyGaugesPanel.h`
- `src/ui/widgets/EnergyGaugesPanel.cpp`

## File NUOVI da aggiungere
- `src/ui/widgets/DayNightCardsRow.h`
- `src/ui/widgets/DayNightCardsRow.cpp`

## CMakeLists.txt
Aggiungi:
```cmake
src/ui/widgets/DayNightCardsRow.cpp
```
(EnergyGaugesPanel.cpp c'è già dal branch precedente, nessuna riga nuova per quello)

## Cosa fare in HomeScreen.h (SOLO aggiungere, non sostituire il file)

Aggiungi l'include vicino agli altri:
```cpp
#include "../widgets/DayNightCardsRow.h"
```

Aggiungi il membro vicino a `StatusCardsRow statusRow;`:
```cpp
DayNightCardsRow dayNightRow;
```

## Cosa fare in HomeScreen.cpp (SOLO aggiungere, non sostituire il file)

Subito dopo la chiamata a `statusRow.create(...)`, aggiungi:
```cpp
dayNightRow.create(
    lv_screen_active(),
    0,
    <Y>,     // <-- subito sotto statusRow, verifica tu la posizione
             //     esatta guardando lo schermo, come fatto finora
    1080,
    140      // altezza indicativa, aggiustabile
);
```

Non ho scelto io il valore di `<Y>` per lo stesso motivo di sempre:
le tue coordinate reali sono cambiate diverse volte e preferisco che
tu verifichi a schermo dove finisce `statusRow` prima di scegliere
il numero esatto, invece di rischiare un'altra sovrapposizione presa
a indovinare.

## Riepilogo delle modifiche fatte

### 1. Card Rete
- Nuova riga sopra "In immissione"/"In prelievo": mostra
  "Pr {importata oggi} / Im {immessa oggi} kWh"
  (sensor.en_importata_sun_oggi, sensor.en_immessa_sun_oggi)
- Testo colorato: rosso se "In prelievo", verde se "In immissione",
  grigio neutro se in equilibrio

### 2. Card Batteria
- Testo sotto al gauge colorato: verde se "Carica", rosso se "Scarica",
  grigio neutro se in equilibrio

### 3. Card Casa
- Aggiunto un gauge più piccolo concentrico dentro l'arco principale,
  che mostra sensor.1p7k_new_potenza
- ATTENZIONE: quel sensore riporta il valore già in kW (non W, a
  differenza degli altri sensori di potenza) — il codice moltiplica
  per 1000 prima di calcolare la percentuale sull'arco (scala 0-3kW,
  regolabile), e mostra il valore originale in kW nell'etichetta
  interna
- Ho spostato in alto l'etichetta del valore principale (potenza
  casa) per non sovrapporsi al gauge interno — verifica a schermo se
  la posizione ti convince, è una stima

### 4. Nuova card Giorno/Notte (DayNightCardsRow)
- 2 card affiancate: "Giorno" (temp/umidità soggiorno) e "Notte"
  (temp/umidità camera), stesso stile visivo delle altre card di stato

## Nota sul gauge interno "Casa"
Il posizionamento a due gauge concentrici nello stesso spazio è
visivamente delicato (rischio di testo/etichette che si accavallano
su una card piccola). Se dopo aver visto il risultato reale ti sembra
troppo affollato, un'alternativa più leggibile potrebbe essere
mostrare il valore di sensor.1p7k_new_potenza come semplice testo
extra sotto alla card invece che come gauge concentrico — fammi
sapere se vuoi che prepari quella variante.
