#!/bin/bash
# Rigenera i font LVGL di src/assets/fonts (richiede node).
#   npm install lv_font_conv @tabler/icons-webfont
#   scarica PT_Sans-Web-Regular/Bold.ttf e PT_Serif-Web-Regular.ttf (google/fonts, licenza OFL) nella cartella corrente
#   crea icon_range.txt con i codepoint delle icone (vedi Icons.h)
#   mkdir out && ./gen_fonts.sh && cp out/*.c ../src/assets/fonts/
set -e
C="node node_modules/lv_font_conv/lv_font_conv.js --bpp 4 --no-compress --format lvgl --lv-include lvgl.h"
LAT="0x20-0x7F,0xA0-0xFF"
for s in 18 20 22 24 26 28; do $C --size $s --font PT_Sans-Web-Regular.ttf --range $LAT --lv-font-name ui_sans_$s -o out/ui_sans_$s.c; done
for s in 22 24 28; do $C --size $s --font PT_Sans-Web-Bold.ttf --range $LAT --lv-font-name ui_sansb_$s -o out/ui_sansb_$s.c; done
for s in 36 40 44 52 60; do $C --size $s --font PT_Serif-Web-Regular.ttf --range $LAT --lv-font-name ui_serif_$s -o out/ui_serif_$s.c; done
$C --size 112 --font PT_Serif-Web-Regular.ttf --range 0x30-0x3A --lv-font-name ui_clock_112 -o out/ui_clock_112.c
ICONS=$(cat icon_range.txt)
for s in 24 40 72; do $C --size $s --font node_modules/@tabler/icons-webfont/dist/fonts/tabler-icons.ttf --range $ICONS --lv-font-name ui_icons_$s -o out/ui_icons_$s.c; done
