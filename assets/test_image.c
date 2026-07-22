#include <lvgl.h>

const uint8_t test_map[] = {
    0xff,0x00,0x00,0xff
};

const lv_image_dsc_t test_image = {
    .header = {
        .magic = LV_IMAGE_HEADER_MAGIC,
        .cf = LV_COLOR_FORMAT_ARGB8888,
        .w = 1,
        .h = 1,
    },
    .data_size = sizeof(test_map),
    .data = test_map,
};