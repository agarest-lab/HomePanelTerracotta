#include "CameraCard.h"

#include <iostream>


void CameraCard::create(
    lv_obj_t* parent,
    int x,
    int y,
    int width,
    int height,
    const std::string& entityId
)
{
    boxWidth = width;
    boxHeight = height;

    card = lv_obj_create(parent);

    lv_obj_set_size(card, width, height);
    lv_obj_set_pos(card, x, y);

    lv_obj_set_style_bg_color(card, lv_color_hex(0x101010), 0);
    lv_obj_set_style_border_width(card, 0, 0);
    lv_obj_set_style_pad_all(card, 0, 0);
    lv_obj_set_style_radius(card, 12, 0);

    lv_obj_clear_flag(card, LV_OBJ_FLAG_SCROLLABLE);


    image = lv_img_create(card);


    // Avvia il download in background (non fa nulla se già avviato
    // per questa entità in precedenza).
    CameraFeed::instance().start(entityId, 3000);


    updateTimer = lv_timer_create(timerCallback, 2000, this);

    refresh();
}


void CameraCard::refresh()
{
    auto frame = CameraFeed::instance().getLatestFrame();

    if(!frame || frame->width <= 0 || frame->height <= 0)
        return; // nessun frame ancora pronto

    if(frame == currentFrame)
        return; // stesso frame già mostrato, niente da fare

    // Mantiene vivo il buffer di pixel finché resta quello mostrato
    currentFrame = frame;

    imgDsc.header.magic = LV_IMAGE_HEADER_MAGIC;
    imgDsc.header.cf = LV_COLOR_FORMAT_ARGB8888;
    imgDsc.header.w = currentFrame->width;
    imgDsc.header.h = currentFrame->height;
    imgDsc.data_size = currentFrame->pixels.size();
    imgDsc.data = currentFrame->pixels.data();

    lv_img_set_src(image, &imgDsc);

    // Scala l'immagine per adattarla alla larghezza del riquadro,
    // mantenendo le proporzioni originali della telecamera.
    float scale = static_cast<float>(boxWidth) / static_cast<float>(currentFrame->width);
    int zoom = static_cast<int>(scale * 256.0f);

    lv_img_set_zoom(image, zoom);

    lv_obj_set_pos(image, 0, 0);
}


void CameraCard::timerCallback(lv_timer_t* timer)
{
    CameraCard* self =
        static_cast<CameraCard*>(
            lv_timer_get_user_data(timer)
        );

    if(self)
    {
        self->refresh();
    }
}
