#include "HomeAssistantRepository.h"

#include <chrono>
#include <iostream>


HomeAssistantRepository& HomeAssistantRepository::instance()
{
    static HomeAssistantRepository repository;
    return repository;
}


HomeAssistantRepository::~HomeAssistantRepository()
{
    stop();
}


void HomeAssistantRepository::watch(const std::string& entityId)
{
    std::lock_guard<std::mutex> lock(watchMutex);

    for(const auto& id : watchedEntities)
    {
        if(id == entityId)
            return; // già registrata, evitiamo duplicati
    }

    watchedEntities.push_back(entityId);
}


void HomeAssistantRepository::start()
{
    if(running)
        return; // già avviato

    running = true;

    pollThread = std::thread(&HomeAssistantRepository::pollLoop, this);
    commandThread = std::thread(&HomeAssistantRepository::commandLoop, this);
}


void HomeAssistantRepository::stop()
{
    if(!running)
        return;

    running = false;

    commandCv.notify_all();

    if(pollThread.joinable())
        pollThread.join();

    if(commandThread.joinable())
        commandThread.join();
}


void HomeAssistantRepository::pollLoop()
{
    while(running)
    {
        std::vector<std::string> entities;

        {
            std::lock_guard<std::mutex> lock(watchMutex);
            entities = watchedEntities;
        }

        for(const auto& entityId : entities)
        {
            if(!running)
                break;

            // Questa è l'unica chiamata di rete bloccante: gira su un
            // thread dedicato, quindi non blocca mai LVGL/SDL.
            std::string value = client.getState(entityId);

            std::lock_guard<std::mutex> lock(stateMutex);
            states[entityId] = value;
        }

        // Attesa suddivisa in piccoli intervalli, così stop() risponde
        // rapidamente invece di dover aspettare fino a 2 secondi pieni.
        for(int i = 0; i < 20 && running; i++)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    }
}


void HomeAssistantRepository::commandLoop()
{
    while(true)
    {
        ServiceCommand cmd;

        {
            std::unique_lock<std::mutex> lock(commandMutex);

            commandCv.wait(lock, [this]
            {
                return !commandQueue.empty() || !running;
            });

            if(!running && commandQueue.empty())
                break;

            cmd = commandQueue.front();
            commandQueue.pop();
        }

        std::cout
            << "Invio comando: "
            << cmd.domain << "." << cmd.service
            << " -> " << cmd.entityId
            << std::endl;

        client.callService(cmd.domain, cmd.service, cmd.entityId);

        // Dopo il comando, rileggiamo lo stato reale per confermare/correggere
        // quello ottimistico già scritto in cache al momento della richiesta.
        std::string confirmedState = client.getState(cmd.entityId);

        if(!confirmedState.empty())
        {
            std::lock_guard<std::mutex> lock(stateMutex);
            states[cmd.entityId] = confirmedState;
        }
    }
}


void HomeAssistantRepository::requestServiceCall(
    const std::string& domain,
    const std::string& service,
    const std::string& entityId,
    const std::string& optimisticState
)
{
    // Aggiorna subito la cache: la UI vedrà il nuovo stato al prossimo
    // refresh, senza dover aspettare la risposta di rete.
    {
        std::lock_guard<std::mutex> lock(stateMutex);
        states[entityId] = optimisticState;
    }

    {
        std::lock_guard<std::mutex> lock(commandMutex);
        commandQueue.push({domain, service, entityId, optimisticState});
    }

    commandCv.notify_one();
}


std::string HomeAssistantRepository::getState(const std::string& entityId) const
{
    std::lock_guard<std::mutex> lock(stateMutex);

    auto it = states.find(entityId);

    if(it == states.end())
        return "";

    return it->second;
}


float HomeAssistantRepository::getFloatState(const std::string& entityId) const
{
    std::string value = getState(entityId);

    if(value.empty() || value == "unknown" || value == "unavailable")
        return 0.0f;

    try
    {
        return std::stof(value);
    }
    catch(const std::exception&)
    {
        return 0.0f;
    }
}


bool HomeAssistantRepository::getBoolState(const std::string& entityId) const
{
    return getState(entityId) == "on";
}
