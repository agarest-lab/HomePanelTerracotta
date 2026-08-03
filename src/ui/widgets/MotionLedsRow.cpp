#include "MotionLedsRow.h"
#include "../../services/HomeAssistantRepository.h"


#define LED_SIZE           36
#define HORIZONTAL_SPACING 140
#define VERTICAL_SPACING   90


void MotionLedsRow::create(
    lv_obj_t* parent,
    int x,
    int y,
    const std::vector<MotionSensorConfig>& configs,
    bool vertical
)
{
    auto& repo = HomeAssistantRepository::instance();

    for(size_t i = 0; i < configs.size(); i++)
    {
        repo.watch(configs[i].entityId);

        int itemX = x;
        int itemY = y;

        if(vertical)
            itemY = y + (int)i * VERTICAL_SPACING;
        else
            itemX = x + (int)i * HORIZONTAL_SPACING;

        lv_obj_t* led = lv_led_create(parent);
        lv_obj_set_size(led, LED_SIZE, LED_SIZE);
        lv_obj_set_pos(led, itemX, itemY);
        lv_led_set_color(led, lv_color_hex(0xFF0000));
        lv_led_set_brightness(led, 60);
        lv_led_off(led);

        lv_obj_t* label = lv_label_create(parent);
        lv_label_set_text(label, configs[i].label.c_str());
        lv_obj_set_style_text_color(label, lv_color_hex(0xD8D8D8), 0);

        if(vertical)
        {
            // Etichetta piccola, centrata sotto al LED, per stare in
            // uno spazio stretto (es. colonna tra due card)
            lv_obj_set_style_text_font(label, &lv_font_montserrat_14, 0);
            lv_obj_set_width(label, 80);
            lv_label_set_long_mode(label, LV_LABEL_LONG_WRAP);
            lv_obj_set_style_text_align(label, LV_TEXT_ALIGN_CENTER, 0);
            lv_obj_align(label, LV_ALIGN_TOP_LEFT, itemX - 22, itemY + LED_SIZE + 4);
        }
        else
        {
            lv_obj_set_style_text_font(label, &lv_font_montserrat_20, 0);
            lv_obj_set_pos(label, itemX - 20, itemY - 30);
        }

        LedEntry entry;
        entry.led = led;
        entry.label = label;
        entry.config = configs[i];

        leds.push_back(entry);
    }

    refresh();

    updateTimer = lv_timer_create(timerCallback, 2000, this);
}


void MotionLedsRow::refresh()
{
    auto& repo = HomeAssistantRepository::instance();

    for(auto& entry : leds)
    {
        bool motionDetected = repo.getBoolState(entry.config.entityId);

        if(motionDetected)
        {
            lv_led_set_brightness(entry.led, 255);
            lv_led_on(entry.led);
        }
        else
        {
            lv_led_set_brightness(entry.led, 60);
            lv_led_off(entry.led);
        }
    }
}


void MotionLedsRow::timerCallback(lv_timer_t* timer)
{
    MotionLedsRow* self =
        static_cast<MotionLedsRow*>(
            lv_timer_get_user_data(timer)
        );

    if(self)
    {
        self->refresh();
    }
}
