# Nuova funzionalità: CameraCard (immagine telecamera in alto a destra)

## File NUOVI da aggiungere
- `src/services/CameraFeed.h`
- `src/services/CameraFeed.cpp`
- `src/ui/widgets/CameraCard.h`
- `src/ui/widgets/CameraCard.cpp`

## File da SOSTITUIRE (contenuto completo)
- `src/ui/UI.h`
- `src/ui/UI.cpp`
- `CMakeLists.txt`

## Dipendenza di sistema da installare PRIMA di compilare

### Su Raspberry Pi / Linux
```bash
sudo apt install -y libjpeg-dev
```

### Su Windows (MSYS2 UCRT64)
```bash
pacman -S mingw-w64-ucrt-x86_64-libjpeg-turbo
```

Senza questa libreria, CMake si fermerà su `find_package(JPEG REQUIRED)`.

## Come funziona
- `CameraFeed` è un singleton con un thread dedicato che scarica lo
  snapshot JPEG da Home Assistant (endpoint `/api/camera_proxy/{entity_id}`)
  ogni 3 secondi, lo decodifica in pixel grezzi (formato ARGB8888, lo
  stesso già usato da `energy_bg`), e lo rende disponibile in modo
  thread-safe.
- `CameraCard` ha un timer LVGL (ogni 2s) che controlla se è disponibile
  un frame nuovo e, in tal caso, lo mostra — scalando l'immagine per
  adattarla al riquadro mantenendo le proporzioni originali della
  telecamera.
- Il thread grafico non fa mai I/O di rete: legge solo un puntatore
  già pronto (shared_ptr), stesso pattern già usato per
  HomeAssistantRepository.

## Posizionamento attuale
- Riquadro: x=560, y=20, larghezza=500, altezza=280
  (metà destra dello schermo, stessa altezza del ClockCard a sinistra)
- Modifica questi numeri in `UI.cpp` se vuoi spostarlo/ridimensionarlo.

## Note
- Il primo frame può richiedere fino a ~3 secondi per essere scaricato:
  è normale vedere il riquadro nero vuoto per un istante all'avvio.
- Se l'entità non risponde (rete, telecamera offline, token scaduto),
  vedrai in console "CameraFeed: richiesta fallita" — il riquadro
  resta semplicemente vuoto finché non arriva un frame valido, senza
  bloccare il resto dell'interfaccia.
