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


// Comando in attesa di essere inviato a Home Assistant
// (accensione/spegnimento di uno switch o di una luce)
struct ServiceCommand
{
    std::string domain;
    std::string service;
    std::string entityId;
    std::string optimisticState; // stato che ci aspettiamo dopo il comando ("on"/"off")
};


// Cache thread-safe verso Home Assistant.
//
// - Un thread di POLLING legge periodicamente lo stato di tutte le entità
//   "osservate" (watch) e lo scrive in una mappa protetta da mutex.
// - Un thread di COMANDI invia le richieste di accensione/spegnimento in coda,
//   senza mai bloccare il thread grafico.
//
// Il thread principale (LVGL/SDL) chiama SOLO i metodi getXState(), che
// leggono dalla cache in memoria e non fanno mai I/O di rete: sono quindi
// sempre veloci e non bloccano mai il rendering o l'input.
class HomeAssistantRepository
{

public:

    static HomeAssistantRepository& instance();


    // Avvia i thread di polling e di invio comandi.
    // Va chiamato una sola volta, dopo aver registrato le entità con watch().
    void start();

    // Ferma i thread. Chiamato automaticamente anche dal distruttore.
    void stop();


    // Registra un'entità da tenere aggiornata dal thread di polling.
    // Può essere chiamato più volte per la stessa entità senza problemi.
    void watch(const std::string& entityId);


    // Letture thread-safe dalla cache (nessun I/O di rete, non bloccano mai)
    std::string getState(const std::string& entityId) const;
    float getFloatState(const std::string& entityId) const;
    bool getBoolState(const std::string& entityId) const;


    // Richiede l'invio di un comando in modo ASINCRONO: ritorna subito,
    // il comando viene eseguito dal thread dedicato. Aggiorna subito anche
    // la cache locale con lo stato atteso (ottimistico), così la UI risponde
    // istantaneamente senza aspettare la conferma di rete.
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
    std::unordered_map<std::string, std::string> states;


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
