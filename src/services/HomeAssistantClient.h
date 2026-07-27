#pragma once
#include <iostream>
#include <string>

class HomeAssistantClient
{

public:

    HomeAssistantClient();


    HomeAssistantClient(
        const std::string& url,
        const std::string& token
    );


    // Ritorna il JSON grezzo di risposta di /api/states/{entityId}
    // (include "state" e "attributes"). Stringa vuota in caso di errore.
    std::string getRawJson(
        const std::string& entityId
    );


    bool callService(
        const std::string& domain,
        const std::string& service,
        const std::string& entityId
    );


    std::string getState(
        const std::string& entityId
    );


    std::string getStringState(
        const std::string& entityId
    );


    float getFloatState(
        const std::string& entityId
    );


    int getIntState(
        const std::string& entityId
    );


private:

    std::string baseUrl;
    std::string token;

};
