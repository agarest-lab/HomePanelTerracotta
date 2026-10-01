#pragma once

#include <lvgl.h>
#include <cstdint>

// Palette "sabbia e terracotta" per il display verticale 1080x1920.
namespace Theme
{
    // Superfici
    constexpr uint32_t BG = 0xEFE7DA, CARD = 0xFBF7EE, CARD_LINE = 0xD9CFBE, LINE = 0xE6DDCB;
    constexpr uint32_t INK = 0x2A2723, MUTED = 0x7A7164, SUB_LIGHT = 0x6A6256, SUB_DARK = 0xF4EFE6;
    constexpr uint32_t WHITE = 0xFFFFFF;

    // Colori delle tessere
    constexpr uint32_t SAND = 0xE4DAC8, SAND_D = 0xCFC4AF;
    constexpr uint32_t TEAL = 0x2E6E6B, OLIVE = 0x5E7A35, BRICK = 0xB8442A, RED = 0xA8412F;
    constexpr uint32_t ORANGE = 0xD9702B, TERRA = 0xC4502D, MUSTARD = 0xE0A526, MUSTARD_L = 0xF2D9A0;
    constexpr uint32_t NIGHT = 0x34505C, CAM = 0x2A2723, LIVE_DOT = 0x8FD16A;

    inline lv_color_t c(uint32_t hex) { return lv_color_hex(hex); }

    // Tono di una tessera: colore di fondo + se e' chiaro (testo scuro) o scuro (testo chiaro).
    // Le tessere DINAMICHE cambiano tono in base ai valori (stessa logica del pannello precedente).
    struct Tone { uint32_t bg; bool light; };

    inline void tint(lv_obj_t* tile, Tone t)
    {
        lv_obj_set_style_bg_color(tile, c(t.bg), 0);
        lv_obj_set_style_bg_opa(tile, LV_OPA_COVER, 0);
    }

    inline void ink(lv_obj_t* l, Tone t, bool sub = false)
    {
        uint32_t col = t.light ? (sub ? SUB_LIGHT : INK) : (sub ? SUB_DARK : WHITE);
        lv_obj_set_style_text_color(l, c(col), 0);
    }

    // Contenitore trasparente flex
    inline lv_obj_t* box(lv_obj_t* parent, lv_flex_flow_t flow,
                         int32_t w = LV_SIZE_CONTENT, int32_t h = LV_SIZE_CONTENT)
    {
        lv_obj_t* o = lv_obj_create(parent);
        lv_obj_remove_style_all(o);
        lv_obj_set_size(o, w, h);
        lv_obj_set_flex_flow(o, flow);
        lv_obj_set_scrollable(o, false);
        lv_obj_set_clickable(o, false);
        return o;
    }

    // Tessera piena (colore deciso poi da tint)
    inline lv_obj_t* tile(lv_obj_t* parent, uint32_t bg)
    {
        lv_obj_t* o = lv_obj_create(parent);
        lv_obj_set_size(o, LV_PCT(100), LV_PCT(100));
        lv_obj_set_style_bg_color(o, c(bg), 0);
        lv_obj_set_style_bg_opa(o, LV_OPA_COVER, 0);
        lv_obj_set_style_border_width(o, 0, 0);
        lv_obj_set_style_radius(o, 26, 0);
        lv_obj_set_style_pad_hor(o, 24, 0);
        lv_obj_set_style_pad_ver(o, 18, 0);
        lv_obj_set_scrollable(o, false);
        return o;
    }

    // Card chiara con bordo
    inline lv_obj_t* card(lv_obj_t* parent)
    {
        lv_obj_t* o = tile(parent, CARD);
        lv_obj_set_style_border_width(o, 2, 0);
        lv_obj_set_style_border_color(o, c(CARD_LINE), 0);
        return o;
    }

    inline lv_obj_t* label(lv_obj_t* parent, const char* text, const lv_font_t* font, uint32_t color = INK)
    {
        lv_obj_t* l = lv_label_create(parent);
        lv_label_set_text(l, text);
        lv_obj_set_style_text_font(l, font, 0);
        lv_obj_set_style_text_color(l, c(color), 0);
        return l;
    }
}
