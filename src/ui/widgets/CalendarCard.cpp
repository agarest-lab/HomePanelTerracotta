#include "CalendarCard.h"
#include "../../services/CalendarFeed.h"

#include <cstdio>
#include <ctime>


lv_color_t CalendarCard::colorForCalendar(const std::string& entityId)
{
    if(entityId == "calendar.omar_rossoni_gmail_com")
        return lv_color_hex(0x4285F4);   // stile Google Calendar

    if(entityId == "calendar.sendina")
        return lv_color_hex(0xFF7043);

    return lv_color_white();
}


// Confronta i primi 10 caratteri di startIso ("YYYY-MM-DD") con la
// data odierna, per capire se l'evento cade oggi.
static bool isToday(const CalendarEvent& ev)
{
    if(ev.startIso.size() < 10)
        return false;

    std::time_t now = std::time(nullptr);
    std::tm nowTm = *std::localtime(&now);

    char todayBuf[11];
    std::strftime(todayBuf, sizeof(todayBuf), "%Y-%m-%d", &nowTm);

    return ev.startIso.compare(0, 10, todayBuf) == 0;
}


void CalendarCard::create(
    lv_obj_t* parent,
    int x,
    int y,
    int width,
    int height
)
{
    card = lv_obj_create(parent);

    lv_obj_set_size(card, width, height);
    lv_obj_set_pos(card, x, y);

    // lv_obj_set_style_bg_color(card, lv_color_hex(0x1A1A1A), 0);
    lv_obj_set_style_bg_color(card, lv_color_hex(0x353535), 0);        // Colore iniziale
    lv_obj_set_style_bg_grad_color(card, lv_color_hex(0x171717), 0);  // Colore finale
    lv_obj_set_style_bg_grad_dir(card, LV_GRAD_DIR_RADIAL, 0);
    lv_obj_set_style_bg_opa(card, LV_OPA_COVER, 0);
    lv_obj_set_style_border_width(card, 0, 0);
    lv_obj_set_style_radius(card, 16, 0);
    lv_obj_set_style_pad_all(card, 16, 0);

    lv_obj_clear_flag(card, LV_OBJ_FLAG_SCROLLABLE);


    lv_obj_t* title = lv_label_create(card);
    lv_label_set_text(title, "Calendario");
    lv_obj_set_style_text_color(title, lv_color_hex(0xD8D8D8), 0);
    lv_obj_set_style_text_font(title, &lv_font_montserrat_36, 0);
    lv_obj_set_pos(title, 0, 0);


    // Container separato per la lista, così a ogni refresh possiamo
    // ripulirlo e ricostruirlo senza toccare il titolo.
    listContainer = lv_obj_create(card);
    lv_obj_set_size(listContainer, width - 32, height - 60);
    lv_obj_set_pos(listContainer, 0, 50);
    lv_obj_set_style_bg_opa(listContainer, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(listContainer, 0, 0);
    lv_obj_set_style_pad_all(listContainer, 0, 0);
    lv_obj_set_flex_flow(listContainer, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(
        listContainer,
        LV_FLEX_ALIGN_START,
        LV_FLEX_ALIGN_START,
        LV_FLEX_ALIGN_START
    );


    auto& feed = CalendarFeed::instance();

    feed.watchCalendar("calendar.omar_rossoni_gmail_com");
    feed.watchCalendar("calendar.sendina");

    feed.start();


    refresh();

    updateTimer = lv_timer_create(timerCallback, 30000, this);
}


void CalendarCard::refresh()
{
    // Ripulisce e ricostruisce la lista (il numero di eventi cambia)
    lv_obj_clean(listContainer);

    auto events = CalendarFeed::instance().getEvents();

    int count = 0;

    for(const auto& ev : events)
    {
        if(count >= maxEvents)
            break;

        lv_obj_t* row = lv_obj_create(listContainer);
        lv_obj_set_size(row, lv_pct(100), LV_SIZE_CONTENT);
        lv_obj_set_style_bg_opa(row, LV_OPA_TRANSP, 0);
        lv_obj_set_style_border_width(row, 0, 0);
        lv_obj_set_style_pad_all(row, 4, 0);
        lv_obj_clear_flag(row, LV_OBJ_FLAG_SCROLLABLE);

        char dateTimeBuf[64];

        if(isToday(ev))
        {
            std::snprintf(
                dateTimeBuf,
                sizeof(dateTimeBuf),
                "OGGI  %s",
                ev.displayTime.c_str()
            );
        }
        else
        {
            std::snprintf(
                dateTimeBuf,
                sizeof(dateTimeBuf),
                "%s  %s",
                ev.displayDate.c_str(),
                ev.displayTime.c_str()
            );
        }

        lv_obj_t* dateLabel = lv_label_create(row);
        lv_label_set_text(dateLabel, dateTimeBuf);
        lv_obj_set_style_text_color(dateLabel, colorForCalendar(ev.calendarId), 0);
        lv_obj_set_style_text_font(dateLabel, &lv_font_montserrat_24, 0);
        lv_obj_set_pos(dateLabel, 0, 0);

        lv_obj_t* summaryLabel = lv_label_create(row);
        lv_label_set_text(summaryLabel, ev.summary.c_str());
        lv_obj_set_style_text_color(summaryLabel, lv_color_white(), 0);
        lv_obj_set_style_text_font(summaryLabel, &lv_font_montserrat_24, 0);
        lv_obj_set_pos(summaryLabel, 290, 0);

        count++;
    }

    if(count == 0)
    {
        lv_obj_t* emptyLabel = lv_label_create(listContainer);
        lv_label_set_text(emptyLabel, "Nessun impegno nei prossimi 7 giorni");
        lv_obj_set_style_text_color(emptyLabel, lv_color_hex(0x808080), 0);
        lv_obj_set_style_text_font(emptyLabel, &lv_font_montserrat_24, 0);
    }
}


void CalendarCard::timerCallback(lv_timer_t* timer)
{
    CalendarCard* self =
        static_cast<CalendarCard*>(
            lv_timer_get_user_data(timer)
        );

    if(self)
    {
        self->refresh();
    }
}