#pragma once

#include <SDL3/SDL.h>
#include <lvgl.h>


class SDLDisplay
{

private:

    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;
    SDL_Texture* texture = nullptr;

    static void flushCallback(
        lv_display_t* display,
        const lv_area_t* area,
        uint8_t* px_map
    );

    lv_indev_t* indev = nullptr;

    static void mouseReadCallback(
        lv_indev_t* indev,
        lv_indev_data_t* data
    );


public:

    bool init();
    void loop();
    ~SDLDisplay();

};