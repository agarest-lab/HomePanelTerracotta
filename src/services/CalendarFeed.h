#pragma once

#include <string>
#include <vector>
#include <mutex>
#include <thread>
#include <atomic>


struct CalendarEvent
{
    std::string summary;
    std::string startIso;       // valore grezzo, usato per l'ordinamento
    std::string displayDate;    // es. "27/07"
    std::string displayTime;    // es. "18:30" oppure "Tutto il giorno"
    bool allDay = false;
    std::string calendarId;     // entity_id del calendario di provenienza
};


// Scarica periodicamente (ogni 5 minuti) gli eventi dei prossimi 7 giorni
// da uno o più calendari Home Assistant, su un thread dedicato.
// Il thread principale legge solo getEvents(), che non fa mai I/O.
class CalendarFeed
{

public:

    static CalendarFeed& instance();

    void start();
    void stop();

    // Registra un calendario da tenere aggiornato (entity_id tipo
    // "calendar.nome_calendario")
    void watchCalendar(const std::string& entityId);

    std::vector<CalendarEvent> getEvents() const;


    CalendarFeed(const CalendarFeed&) = delete;
    CalendarFeed& operator=(const CalendarFeed&) = delete;


private:

    CalendarFeed() = default;
    ~CalendarFeed();

    void loop();

    std::vector<CalendarEvent> fetchEventsForCalendar(const std::string& entityId);


    std::vector<std::string> calendarIds;
    mutable std::mutex idsMutex;

    std::atomic<bool> running{false};
    std::thread worker;

    mutable std::mutex eventsMutex;
    std::vector<CalendarEvent> events;

};
