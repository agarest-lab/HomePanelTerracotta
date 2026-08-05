#include "SDLDisplay.h"

#include <iostream>


static constexpr int WIDTH = 1080;
static constexpr int HEIGHT = 1920;
static uint8_t buffer[1080 * 1920 * 4];


static uint32_t getTickCallback()
{
    return static_cast<uint32_t>(SDL_GetTicks());
}


bool SDLDisplay::init()
{
    lv_init();
    lv_tick_set_cb(getTickCallback);

    lv_display_t* display;

    display = lv_display_create(
        WIDTH,
        HEIGHT
    );

    if(!display)
    {
        std::cout << "lv_display_create failed" << std::endl;
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

    indev = lv_indev_create();
    lv_indev_set_type(indev, LV_INDEV_TYPE_POINTER);
    lv_indev_set_read_cb(indev, mouseReadCallback);

    if(!SDL_Init(SDL_INIT_VIDEO))
    {
        std::cout << "SDL_Init failed: " << SDL_GetError() << std::endl;
        return false;
    }

    window = SDL_CreateWindow(
        "HomePanel",
        WIDTH,
        HEIGHT,
        SDL_WINDOW_FULLSCREEN
    );

    if(!window)
    {
        std::cout << "SDL_CreateWindow failed: " << SDL_GetError() << std::endl;
        return false;
    }

    renderer = SDL_CreateRenderer(window, nullptr);

    if(!renderer)
    {
        std::cout << "SDL_CreateRenderer failed: " << SDL_GetError() << std::endl;
        return false;
    }

    texture = SDL_CreateTexture(
        renderer,
        SDL_PIXELFORMAT_ARGB8888,
        SDL_TEXTUREACCESS_STREAMING,
        WIDTH,
        HEIGHT
    );

    if(!texture)
    {
        std::cout << "SDL_CreateTexture failed: " << SDL_GetError() << std::endl;
        return false;
    }

    SDL_SetTextureBlendMode(
        texture,
        SDL_BLENDMODE_NONE
    );

    std::cout << "SDLDisplay init OK (" << WIDTH << "x" << HEIGHT << ")" << std::endl;

    return true;
}


void SDLDisplay::mouseReadCallback(
    lv_indev_t* indev,
    lv_indev_data_t* data
)
{
    float x, y;
    Uint32 buttons = SDL_GetMouseState(&x, &y);

    data->point.x = (lv_coord_t)x;
    data->point.y = (lv_coord_t)y;

    data->state = (buttons & SDL_BUTTON_MASK(SDL_BUTTON_LEFT))
        ? LV_INDEV_STATE_PRESSED
        : LV_INDEV_STATE_RELEASED;

    if(data->state == LV_INDEV_STATE_PRESSED)
    {
        std::cout << "Mouse PRESSED at " << x << "," << y << std::endl;
    }
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

    lv_timer_handler();

    SDL_Delay(5);
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