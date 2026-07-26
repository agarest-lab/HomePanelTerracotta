# Nuova funzionalità: riga di 4 card di stato (condensa/ACS/impianto/allarme)

## File NUOVI da aggiungere
- `src/ui/widgets/StatusCardsRow.h`
- `src/ui/widgets/StatusCardsRow.cpp`

## File da SOSTITUIRE (contenuto completo)
- `src/ui/screens/HomeScreen.h`
- `src/ui/screens/HomeScreen.cpp`
- `CMakeLists.txt`

## Entità Home Assistant usate
- sensor.ur_limite_anticondensa
- sensor.acs_pdc
- sensor.acs_casa
- sensor.mandata_pdc
- sensor.ritorno_casa
- alarm_control_panel.80_000_zone_1_alarm

Verifica che questi entity_id esistano esattamente così nella tua
istanza Home Assistant (stesso discorso già fatto per light.cuoricini).

## Note sul posizionamento
Il riquadro è posizionato a (0, 1310), larghezza 1080, altezza 180 —
subito sotto la riga delle prese. Se non combacia perfettamente con
il resto del layout sul tuo schermo, modifica questi 4 numeri nella
chiamata a `statusRow.create(...)` dentro `HomeScreen::create()`.

## Cosa manca rispetto alla dashboard Home Assistant originale
- Le icone MDI (termometro, scudo, ecc.) non sono incluse: richiederebbero
  un font icone dedicato da generare a parte (stesso procedimento usato
  per il font grande dell'orologio, ma con un set di glifi icona invece
  che testo). Se le vuoi, si può aggiungere in un secondo momento.
- I colori/soglie sono gli stessi della dashboard originale (verde se
  acs_pdc >= 34°C, altrimenti rosso), ma i toni esatti sono stati
  adattati per un buon contrasto su sfondo nero anziché replicare le
  trasparenze CSS originali (rgba con box-shadow "glow") — è puramente
  estetico, la logica è identica.
