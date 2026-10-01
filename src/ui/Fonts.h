#pragma once
#include <lvgl.h>

// Font generati con lv_font_conv (vedi README_PORTRAIT.md):
//  - ui_sans_*  / ui_sansb_*  : PT Sans (Latin-1: accenti, °, µ)
//  - ui_serif_* / ui_clock_112: PT Serif per numeri e orologio
//  - ui_icons_*               : icone Tabler (vedi Icons.h)
#ifdef __cplusplus
extern "C" {
#endif

extern const lv_font_t ui_sans_18, ui_sans_20, ui_sans_22, ui_sans_24, ui_sans_26, ui_sans_28;
extern const lv_font_t ui_sansb_22, ui_sansb_24, ui_sansb_28;
extern const lv_font_t ui_serif_36, ui_serif_40, ui_serif_44, ui_serif_52, ui_serif_60;
extern const lv_font_t ui_clock_112;
extern const lv_font_t ui_icons_24, ui_icons_40, ui_icons_72;

#ifdef __cplusplus
}
#endif
