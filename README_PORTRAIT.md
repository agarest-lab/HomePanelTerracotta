# HomePanel - interfaccia verticale 1080x1920 "sabbia e terracotta" (base: tag v1.3.2)

Decomprimi nella root del repo (sovrascrive i file elencati) e ricompila.

## File
- NUOVI: `src/ui/Theme.h` (palette e helper), `src/ui/Fonts.h`, `src/ui/Icons.h`,
  `src/ui/widgets/PanelWidgets.{h,cpp}`, `src/assets/fonts/*.c` (18 font generati), `tools/gen_fonts.sh`
- MODIFICATI: `CMakeLists.txt`, `src/ui/UI.{h,cpp}`, `src/ui/screens/HomeScreen.{h,cpp}`,
  `src/ui/widgets/CameraCard.cpp` (scala "cover" e centra il video)

## Font e icone
- PT Sans (testi, con accenti), PT Serif (numeri e orologio): licenza OFL.
- Icone Tabler (outline): licenza MIT. Per aggiungerne, modifica `icon_range.txt` e `Icons.h`, poi `tools/gen_fonts.sh`.

## Colori dinamici
Stessa logica di prima, nuova palette (`Theme::Tone`): batteria per SOC, rete per segno, fotovoltaico a riposo/in produzione,
condensa (pompa > 150 W), ACS (>= 34 C), allarme per stato, meteo per temperatura, AQI e Stazione per soglie, allerta per livello meteoalarm.

## Non presenti (file ancora nel repo, non istanziati)
Bagno, lavatrice, LED movimento, T ingresso.
