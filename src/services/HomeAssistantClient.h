#pragma once

#include <string>

class HomeAssistantClient
{
private:

    std::string baseUrl;
    std::string token;

    std::string getState(const std::string& entityId);

public:

    HomeAssistantClient(
        const std::string& url,
        const std::string& token
    );

    std::string getStringState(const std::string& entityId);

    float getFloatState(const std::string& entityId);

    int getIntState(const std::string& entityId);
};