#include "HomeAssistantClient.h"

#include <curl/curl.h>
#include <nlohmann/json.hpp>

#include <iostream>

using json = nlohmann::json;

static size_t WriteCallback(
    void* contents,
    size_t size,
    size_t nmemb,
    void* userp
)
{
    ((std::string*)userp)->append(
        (char*)contents,
        size * nmemb
    );

    return size * nmemb;
}

HomeAssistantClient::HomeAssistantClient(
    const std::string& url,
    const std::string& token
)
{
    baseUrl = url;
    this->token = token;
}

std::string HomeAssistantClient::getState(
    const std::string& entityId
)
{
    CURL* curl = curl_easy_init();

    if(!curl)
        return "";

    std::string response;

    std::string url =
        baseUrl + "/api/states/" + entityId;

    struct curl_slist* headers = nullptr;

    headers = curl_slist_append(
        headers,
        ("Authorization: Bearer " + token).c_str()
    );

    headers = curl_slist_append(
        headers,
        "Content-Type: application/json"
    );

    curl_easy_setopt(
        curl,
        CURLOPT_URL,
        url.c_str()
    );

    curl_easy_setopt(
        curl,
        CURLOPT_HTTPHEADER,
        headers
    );

    curl_easy_setopt(
        curl,
        CURLOPT_WRITEFUNCTION,
        WriteCallback
    );

    curl_easy_setopt(
        curl,
        CURLOPT_WRITEDATA,
        &response
    );

    CURLcode res = curl_easy_perform(curl);

    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);

    if(res != CURLE_OK)
    {
        std::cout
            << "curl error: "
            << curl_easy_strerror(res)
            << std::endl;

        return "";
    }

    try
    {
        json data = json::parse(response);

        return data["state"].get<std::string>();
    }
    catch(...)
    {
        std::cout
            << "JSON parse error"
            << std::endl;

        return "";
    }
}

std::string HomeAssistantClient::getStringState(
    const std::string& entityId
)
{
    return getState(entityId);
}

float HomeAssistantClient::getFloatState(
    const std::string& entityId
)
{
    return std::stof(
        getState(entityId)
    );
}

int HomeAssistantClient::getIntState(
    const std::string& entityId
)
{
    return std::stoi(
        getState(entityId)
    );
}