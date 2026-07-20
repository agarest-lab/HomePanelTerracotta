#include "SDLDisplay.h"

#include <iostream>


static constexpr int WIDTH = 600;
static constexpr int HEIGHT = 1024;
static lv_color_t buffer[600 * 1024];



bool SDLDisplay::init()
{
    std::cout << "SDLDisplay init start" << std::endl;
    lv_init();
    std::cout << "LVGL init done" << std::endl;
    lv_display_t* display;


    display = lv_display_create(
        WIDTH,
        HEIGHT
    );

    std::cout << "LV display created" << std::endl;
  
    lv_display_set_user_data(
        display,
        this
    );

    lv_display_set_flush_cb(
        display,
        flushCallback
    );
    std::cout << "Flush callback set" << std::endl;
    std::cout << "Before buffers" << std::endl;
    lv_display_set_buffers(
        display,
        buffer,
        nullptr,
        sizeof(buffer),
        LV_DISPLAY_RENDER_MODE_PARTIAL
    );
    std::cout << "After buffers" << std::endl;
    std::cout << "LV buffers set" << std::endl;
    
    if(!SDL_Init(SDL_INIT_VIDEO))
    
    {
        std::cout
            << SDL_GetError()
            << std::endl;

        return false;
    }


    window = SDL_CreateWindow(
        "HomePanel",
        WIDTH,
        HEIGHT,
        0
    );


    if(!window)
    {
        return false;
    }


    renderer = SDL_CreateRenderer(window, nullptr);


    texture = SDL_CreateTexture(
        renderer,
        SDL_PIXELFORMAT_XRGB8888,
        SDL_TEXTUREACCESS_STREAMING,
        WIDTH,
        HEIGHT
    );


    std::cout 
        << "LVGL + SDL initialized"
        << std::endl;

    std::cout << "SDLDisplay init finished" << std::endl;
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


    SDL_UpdateTexture(
        self->texture,
        nullptr,
        px_map,
        600 * sizeof(lv_color_t)
    );


    SDL_RenderClear(self->renderer);


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


    lv_timer_handler();


    SDL_Delay(5);

}