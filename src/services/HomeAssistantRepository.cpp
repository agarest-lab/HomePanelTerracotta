#include "HomeAssistantRepository.h"
#include <nlohmann/json.hpp>
#include <chrono>
#include <iostream>

using json = nlohmann::json;

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
            return;
    }

    watchedEntities.push_back(entityId);
}

void HomeAssistantRepository::start()
{
    if(running)
        return;

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

            std::string rawJson = client.getRawJson(entityId);

            if(!rawJson.empty())
            {
                std::lock_guard<std::mutex> lock(stateMutex);
                rawJsonCache[entityId] = rawJson;
            }
        }

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

        client.callService(cmd.domain, cmd.service, cmd.entityId);

        std::string confirmedJson = client.getRawJson(cmd.entityId);

        if(!confirmedJson.empty())
        {
            std::lock_guard<std::mutex> lock(stateMutex);
            rawJsonCache[cmd.entityId] = confirmedJson;
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
    // Costruisce un JSON minimale "ottimistico" così getState() continua
    // a funzionare subito, senza aspettare la conferma di rete.
    {
        std::lock_guard<std::mutex> lock(stateMutex);

        json optimistic;
        optimistic["state"] = optimisticState;
        optimistic["attributes"] = json::object();

        rawJsonCache[entityId] = optimistic.dump();
    }

    {
        std::lock_guard<std::mutex> lock(commandMutex);
        commandQueue.push({domain, service, entityId, optimisticState});
    }

    commandCv.notify_one();
}

std::string HomeAssistantRepository::getState(const std::string& entityId) const
{
    std::string rawJson;

    {
        std::lock_guard<std::mutex> lock(stateMutex);

        auto it = rawJsonCache.find(entityId);

        if(it == rawJsonCache.end())
            return "";

        rawJson = it->second;
    }

    try
    {
        json data = json::parse(rawJson);

        if(!data.contains("state"))
            return "";

        return data["state"].get<std::string>();
    }
    catch(const std::exception&)
    {
        return "";
    }
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

std::string HomeAssistantRepository::getAttribute(
    const std::string& entityId,
    const std::string& attributeName
) const
{
    std::string rawJson;

    {
        std::lock_guard<std::mutex> lock(stateMutex);

        auto it = rawJsonCache.find(entityId);

        if(it == rawJsonCache.end())
            return "";

        rawJson = it->second;
    }

    try
    {
        json data = json::parse(rawJson);

        if(!data.contains("attributes"))
            return "";

        const json& attributes = data["attributes"];

        if(!attributes.contains(attributeName))
            return "";

        const json& value = attributes[attributeName];

        if(value.is_string())
            return value.get<std::string>();

        if(value.is_null())
            return "";

        // Numeri, bool, ecc: rappresentazione testuale semplice
        return value.dump();
    }
    catch(const std::exception&)
    {
        return "";
    }
}
