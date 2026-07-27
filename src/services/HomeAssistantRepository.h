#pragma once

#include <string>
#include <unordered_map>
#include <vector>
#include <queue>
#include <mutex>
#include <thread>
#include <atomic>
#include <condition_variable>

#include "HomeAssistantClient.h"


struct ServiceCommand
{
    std::string domain;
    std::string service;
    std::string entityId;
    std::string optimisticState;
};


// Cache thread-safe verso Home Assistant.
//
// A differenza della prima versione, ora conserva il JSON COMPLETO di
// ogni entità (non solo lo "state"), così da poter esporre anche gli
// attributi (es. "expires", "event", "awareness_level" di un'entità
// meteo-allerta) tramite getAttribute().
//
// - Un thread di POLLING legge periodicamente il JSON completo di tutte
//   le entità "osservate" (watch).
// - Un thread di COMANDI invia le richieste di accensione/spegnimento.
//
// Il thread principale (LVGL/SDL) chiama solo i metodi getXxx(), che
// leggono dalla cache in memoria e non fanno mai I/O di rete.
class HomeAssistantRepository
{

public:

    static HomeAssistantRepository& instance();

    void start();
    void stop();

    void watch(const std::string& entityId);


    // Letture thread-safe dalla cache (nessun I/O di rete)
    std::string getState(const std::string& entityId) const;
    float getFloatState(const std::string& entityId) const;
    bool getBoolState(const std::string& entityId) const;

    // Legge un attributo dell'entità (es. "expires", "event").
    // Ritorna stringa vuota se l'entità o l'attributo non sono presenti.
    std::string getAttribute(
        const std::string& entityId,
        const std::string& attributeName
    ) const;


    void requestServiceCall(
        const std::string& domain,
        const std::string& service,
        const std::string& entityId,
        const std::string& optimisticState
    );


    HomeAssistantRepository(const HomeAssistantRepository&) = delete;
    HomeAssistantRepository& operator=(const HomeAssistantRepository&) = delete;


private:

    HomeAssistantRepository() = default;
    ~HomeAssistantRepository();

    HomeAssistantClient client;


    mutable std::mutex stateMutex;

    // Per ogni entity_id, il JSON grezzo completo restituito da HA
    std::unordered_map<std::string, std::string> rawJsonCache;


    mutable std::mutex watchMutex;
    std::vector<std::string> watchedEntities;


    std::atomic<bool> running{false};


    std::thread pollThread;
    void pollLoop();


    std::thread commandThread;
    std::mutex commandMutex;
    std::condition_variable commandCv;
    std::queue<ServiceCommand> commandQueue;
    void commandLoop();

};
