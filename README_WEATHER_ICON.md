# Nuova card icona meteo animata + Giorno/Notte ridotte

## File NUOVI da aggiungere
- `src/ui/widgets/WeatherIconCard.h`
- `src/ui/widgets/WeatherIconCard.cpp`

## File da SOSTITUIRE per intero
- `src/ui/widgets/DayNightCardsRow.h`
- `src/ui/widgets/DayNightCardsRow.cpp`
  (la firma di create() è cambiata: ora prende la larghezza di UNA
  singola card invece della larghezza totale della riga, dato che ora
  fa parte di una riga composita con altri widget)

## CMakeLists.txt
Aggiungi:
```cmake
src/ui/widgets/WeatherIconCard.cpp
```

## HomeScreen.h — aggiungi (non sostituire il file)
```cpp
#include "../widgets/WeatherIconCard.h"
```
```cpp
WeatherIconCard weatherIcon;
```

## HomeScreen.cpp — sostituisci la vecchia chiamata a dayNightRow.create(...)

Con la card Auto fissa a 350px di larghezza, ecco i calcoli per far
stare tutto in 1080px (icona + Giorno + Notte + Auto):

```cpp
// Icona meteo animata, a sinistra di "Giorno"
weatherIcon.create(
    lv_screen_active(),
    0,
    <Y>,     // <-- stessa Y che usavi per dayNightRow
    227,
    180      // <-- stessa altezza che usavi per dayNightRow
);

// Giorno + Notte, ridotte, subito dopo l'icona
dayNightRow.create(
    lv_screen_active(),
    239,     // 227 + 12 di gap
    <Y>,     // <-- stessa Y di sopra
    227,     // larghezza di CIASCUNA delle due card (Giorno e Notte)
    180      // <-- stessa altezza di sopra
);

// Auto, invariata (dimensione e posizione che avevi già)
carStatusCard.create(
    lv_screen_active(),
    730,     // già usata prima, invariata
    <Y>,
    350,
    180
);
```

Nota: `239 + 227 + 12 + 227 = 705`, e la card "Notte" (interna a
dayNightRow) occupa fino a `705 + 227 = 932`; con un gap di 12 prima
di "Auto" a x=730... in realtà **verifica tu i numeri esatti** una
volta viste le card a schermo — la somma teorica (227×3 + 12×3 + 350
= 1067) lascia un margine di ~13px non usato sul lato destro, che è
trascurabile ma potresti voler distribuire diversamente (es.
aumentare leggermente i gap, o le 3 colonne strette).

## Come funziona WeatherIconCard

Legge `weather.openweathermap` e mappa lo stato in 4 categorie:
- **Sereno** (sunny, clear-night): cerchio giallo con animazione di
  "respiro" (opacità che pulsa dolcemente)
- **Nuvoloso** (cloudy, partlycloudy, fog, windy): nuvola grigia con
  leggera deriva orizzontale
- **Pioggia/neve** (rainy, pouring, snowy, hail, snowy-rainy): nuvola
  + gocce blu che cadono in loop, sfalsate
- **Temporale** (lightning, lightning-rainy, exceptional): nuvola
  scura + fulmine giallo che lampeggia

Tutte le forme sono disegnate con primitive LVGL (cerchi/rettangoli
arrotondati), nessun font/immagine esterna. Le animazioni usano solo
opacità e posizione X/Y (mai `transform_zoom` o rotazioni), per
evitare i problemi di stabilità già incontrati in passato con quelle
trasformazioni su questo setup.

L'icona si ricostruisce (e l'animazione riparte) solo quando la
categoria meteo cambia, non ad ogni refresh di 5s — altrimenti
l'animazione "scatterebbe" continuamente.
