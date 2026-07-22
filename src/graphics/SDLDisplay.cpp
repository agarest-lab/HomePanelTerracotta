#include "SDLDisplay.h"

#include <iostream>


static constexpr int WIDTH = 600;
static constexpr int HEIGHT = 1024;
static uint8_t buffer[600 * 1024 * 4];



bool SDLDisplay::init()
{

    lv_init();

    lv_display_t* display;


    display = lv_display_create(
        WIDTH,
        HEIGHT
    );

    if(!display)
    {
        return false;
    }
  
    lv_display_set_user_data(
        display,
        this
    );

    lv_display_set_flush_cb(
        display,
        flushCallback
    );

    lv_display_set_buffers(
        display,
        buffer,
        nullptr,
        sizeof(buffer),
        LV_DISPLAY_RENDER_MODE_FULL
    );
    
    if(!SDL_Init(SDL_INIT_VIDEO))
    {

        return false;
    }

    window = SDL_CreateWindow(
        "HomePanel",
        WIDTH,
        HEIGHT,
        0
    );

    renderer = SDL_CreateRenderer(window, nullptr);

    texture = SDL_CreateTexture(
        renderer,
        SDL_PIXELFORMAT_ARGB8888,
        SDL_TEXTUREACCESS_STREAMING,
        WIDTH,
        HEIGHT
    );

    SDL_SetTextureBlendMode(
        texture,
        SDL_BLENDMODE_NONE
    );

    return true;
}

void SDLDisplay::flushCallback(
    lv_display_t* display,
    const lv_area_t* area,
    uint8_t* px_map

)

{

    SDLDisplay* self =
        static_cast<SDLDisplay*>(
            lv_display_get_user_data(display)
        );

    static int counter = 0;

    SDL_Rect rect;

    rect.x = area->x1;
    rect.y = area->y1;
    rect.w = area->x2 - area->x1 + 1;
    rect.h = area->y2 - area->y1 + 1;


    SDL_UpdateTexture(
        self->texture,
        nullptr,
        px_map,
        WIDTH * 4
    );


    SDL_RenderClear(
        self->renderer
    );


    SDL_RenderTexture(
        self->renderer,
        self->texture,
        nullptr,
        nullptr
    );


    SDL_RenderPresent(
        self->renderer
    );


    lv_display_flush_ready(display);
}

void SDLDisplay::loop()
{
    SDL_Event event;


    while(SDL_PollEvent(&event))
    {
        if(event.type == SDL_EVENT_QUIT)
        {
            exit(0);
        }
    }


    uint32_t delay = lv_timer_handler();


    SDL_Delay(
        delay > 5 ? delay : 5
    );
}

SDLDisplay::~SDLDisplay()
{
    if(texture)
        SDL_DestroyTexture(texture);

    if(renderer)
        SDL_DestroyRenderer(renderer);

    if(window)
        SDL_DestroyWindow(window);

    SDL_Quit();
}