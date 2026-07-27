#include "CalendarFeed.h"
#include "../config/secrets.h"

#include <curl/curl.h>
#include <nlohmann/json.hpp>

#include <ctime>
#include <chrono>
#include <algorithm>
#include <iostream>


using json = nlohmann::json;


static size_t WriteStringCallback(
    void* contents,
    size_t size,
    size_t nmemb,
    void* userp
)
{
    size_t totalSize = size * nmemb;

    auto* buffer = static_cast<std::string*>(userp);

    buffer->append(
        static_cast<char*>(contents),
        totalSize
    );

    return totalSize;
}


CalendarFeed& CalendarFeed::instance()
{
    static CalendarFeed feed;
    return feed;
}


CalendarFeed::~CalendarFeed()
{
    stop();
}


void CalendarFeed::watchCalendar(const std::string& entityId)
{
    std::lock_guard<std::mutex> lock(idsMutex);

    for(const auto& id : calendarIds)
    {
        if(id == entityId)
            return;
    }

    calendarIds.push_back(entityId);
}


void CalendarFeed::start()
{
    if(running)
        return;

    running = true;

    worker = std::thread(&CalendarFeed::loop, this);
}


void CalendarFeed::stop()
{
    if(!running)
        return;

    running = false;

    if(worker.joinable())
        worker.join();
}


std::vector<CalendarEvent> CalendarFeed::fetchEventsForCalendar(
    const std::string& entityId
)
{
    std::vector<CalendarEvent> result;

    // Intervallo: da oggi 00:00 ai prossimi 7 giorni
    std::time_t now = std::time(nullptr);
    std::tm startTm = *std::localtime(&now);
    startTm.tm_hour = 0;
    startTm.tm_min = 0;
    startTm.tm_sec = 0;

    std::time_t startTime = std::mktime(&startTm);
    std::time_t endTime = startTime + 7 * 24 * 3600;
    std::tm endTm = *std::localtime(&endTime);

    char startBuf[32];
    char endBuf[32];

    std::strftime(startBuf, sizeof(startBuf), "%Y-%m-%dT%H:%M:%S", &startTm);
    std::strftime(endBuf, sizeof(endBuf), "%Y-%m-%dT%H:%M:%S", &endTm);


    CURL* curl = curl_easy_init();

    if(!curl)
        return result;

    char* startEnc = curl_easy_escape(curl, startBuf, 0);
    char* endEnc = curl_easy_escape(curl, endBuf, 0);

    std::string url =
        std::string(HA_URL) +
        "/api/calendars/" +
        entityId +
        "?start=" + startEnc +
        "&end=" + endEnc;

    curl_free(startEnc);
    curl_free(endEnc);


    std::string response;
    struct curl_slist* headers = nullptr;

    std::string auth = "Authorization: Bearer " + std::string(HA_TOKEN);
    headers = curl_slist_append(headers, auth.c_str());

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteStringCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 8L);

    CURLcode res = curl_easy_perform(curl);

    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);

    if(res != CURLE_OK || response.empty())
    {
        std::cout
            << "CalendarFeed: richiesta fallita per "
            << entityId
            << std::endl;

        return result;
    }


    try
    {
        json data = json::parse(response);

        for(auto& item : data)
        {
            CalendarEvent ev;
            ev.calendarId = entityId;
            ev.summary = item.value("summary", "(senza titolo)");

            if(item.contains("start"))
            {
                if(item["start"].contains("dateTime"))
                {
                    ev.startIso = item["start"]["dateTime"].get<std::string>();
                    ev.allDay = false;
                }
                else if(item["start"].contains("date"))
                {
                    ev.startIso = item["start"]["date"].get<std::string>();
                    ev.allDay = true;
                }
            }

            if(ev.startIso.size() >= 10)
            {
                std::string dd = ev.startIso.substr(8, 2);
                std::string mm = ev.startIso.substr(5, 2);
                ev.displayDate = dd + "/" + mm;
            }

            if(!ev.allDay && ev.startIso.size() >= 16)
            {
                ev.displayTime = ev.startIso.substr(11, 5);
            }
            else
            {
                ev.displayTime = "Tutto il giorno";
            }

            result.push_back(ev);
        }
    }
    catch(const std::exception& e)
    {
        std::cout
            << "CalendarFeed: parsing fallito per "
            << entityId
            << ": "
            << e.what()
            << std::endl;
    }

    return result;
}


void CalendarFeed::loop()
{
    while(running)
    {
        std::vector<std::string> ids;

        {
            std::lock_guard<std::mutex> lock(idsMutex);
            ids = calendarIds;
        }

        std::vector<CalendarEvent> allEvents;

        for(const auto& id : ids)
        {
            if(!running)
                break;

            auto calendarEvents = fetchEventsForCalendar(id);

            allEvents.insert(
                allEvents.end(),
                calendarEvents.begin(),
                calendarEvents.end()
            );
        }

        std::sort(
            allEvents.begin(),
            allEvents.end(),
            [](const CalendarEvent& a, const CalendarEvent& b)
            {
                return a.startIso < b.startIso;
            }
        );

        {
            std::lock_guard<std::mutex> lock(eventsMutex);
            events = allEvents;
        }

        // Aggiornamento ogni 5 minuti (i calendari cambiano di rado)
        int waited = 0;
        while(waited < 5 * 60 * 1000 && running)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(200));
            waited += 200;
        }
    }
}


std::vector<CalendarEvent> CalendarFeed::getEvents() const
{
    std::lock_guard<std::mutex> lock(eventsMutex);
    return events;
}
