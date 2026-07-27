# Nuova funzionalità: card impegni calendario

## File NUOVI da aggiungere
- `src/services/CalendarFeed.h`
- `src/services/CalendarFeed.cpp`
- `src/ui/widgets/CalendarCard.h`
- `src/ui/widgets/CalendarCard.cpp`

## CMakeLists.txt
Aggiungi entrambe le righe:
```cmake
src/services/CalendarFeed.cpp

src/ui/widgets/CalendarCard.cpp
```

## Come funziona
- `CalendarFeed` (singleton, thread dedicato) interroga ogni 5 minuti
  l'endpoint `/api/calendars/{entity_id}/events` di Home Assistant per
  ogni calendario registrato, richiedendo gli eventi dei prossimi 7
  giorni. Aggiorna una lista combinata e ordinata cronologicamente.
- `CalendarCard` mostra fino a 6 eventi, con data/ora a sinistra
  (colorata in base al calendario di provenienza) e titolo a destra.
  Se non ci sono impegni, mostra un messaggio.

## Come integrare in HomeScreen

In `HomeScreen.h`:
```cpp
#include "../widgets/CalendarCard.h"
...
CalendarCard calendarCard;
```

In `HomeScreen.cpp`, dentro `create()`, in fondo (per ultima, come
richiesto):
```cpp
calendarCard.create(
    lv_screen_active(),
    0,
    <Y>,     // <-- subito sotto weatherRow, verifica tu la posizione
             //     esatta a schermo prima di scegliere il numero,
             //     come già fatto per gli altri blocchi
    1080,
    220      // altezza: 6 righe di eventi + titolo, aggiustabile
);
```

## Limitazioni note
- **Niente meteo integrato** nella card (lo YAML originale mostrava
  condizioni/temperature accanto alla data) — è una funzionalità
  separata che richiede leggere le previsioni dall'entità
  `weather.openweathermap` (attributo `forecast`, che è un array di
  previsioni giornaliere). Si può aggiungere in un secondo momento.
- **Colori dei calendari**: ho impostato un blu per
  `calendar.omar_rossoni_gmail_com` e un arancione per
  `calendar.sendina` (nello YAML originale il colore di quest'ultimo
  era vuoto/non specificato, quindi ho scelto un colore di default).
  Modifica `colorForCalendar()` in `CalendarCard.cpp` se vuoi altri colori.
- **Orari**: come per l'allerta meteo, gli orari vengono mostrati così
  come li restituisce Home Assistant, senza conversione esplicita di
  fuso orario lato codice — nella pratica Home Assistant restituisce
  già gli orari nel fuso configurato sul server, quindi nella maggior
  parte dei casi coincide con l'ora locale attesa.
- **Eventi "tutto il giorno"**: mostrati con l'etichetta "Tutto il
  giorno" invece di un orario specifico.
- Nessuna riga verticale colorata a sinistra come nello YAML originale
  (vertical_line_width) — qui distinguo i calendari solo tramite il
  colore del testo data/ora. Si può aggiungere una barra colorata se
  preferisci quell'estetica.
