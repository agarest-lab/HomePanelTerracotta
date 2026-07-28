#include "HomeAssistantClient.h"
#include "../config/secrets.h"
#include <curl/curl.h>
#include <nlohmann/json.hpp>

#include <iostream>
#include <string>


using json = nlohmann::json;


static size_t WriteCallback(
    void* contents,
    size_t size,
    size_t nmemb,
    void* userp
)
{
    size_t totalSize = size * nmemb;

    std::string* response =
        static_cast<std::string*>(userp);

    response->append(
        static_cast<char*>(contents),
        totalSize
    );

    return totalSize;
}

HomeAssistantClient::HomeAssistantClient()
{
    baseUrl = HA_URL;
    token = HA_TOKEN;
}



HomeAssistantClient::HomeAssistantClient(
    const std::string& url,
    const std::string& token
)
{
    baseUrl = url;
    this->token = token;
}


std::string HomeAssistantClient::getRawJson(
    const std::string& entityId
)
{
    CURL* curl = curl_easy_init();

    if(!curl)
    {
        std::cout
            << "curl init failed"
            << std::endl;

        return "";
    }


    std::string response;


    std::string url =
        baseUrl +
        "/api/states/" +
        entityId;

    struct curl_slist* headers = nullptr;


    std::string auth =
        "Authorization: Bearer " + token;


    headers = curl_slist_append(
        headers,
        auth.c_str()
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


    curl_easy_setopt(
        curl,
        CURLOPT_TIMEOUT,
        5L
    );



    CURLcode result =
        curl_easy_perform(curl);


    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);



    if(result != CURLE_OK)
    {
        std::cout
            << "curl error: "
            << curl_easy_strerror(result)
            << std::endl;

        return "";
    }


    return response;
}


bool HomeAssistantClient::callService(
    const std::string& domain,
    const std::string& service,
    const std::string& entityId
)
{
    CURL* curl = curl_easy_init();
    if(!curl) return false;

    std::string url = baseUrl + "/api/services/" + domain + "/" + service;

    json body;
    body["entity_id"] = entityId;
    std::string payload = body.dump();

    std::string response;
    struct curl_slist* headers = nullptr;
    std::string auth = "Authorization: Bearer " + token;
    headers = curl_slist_append(headers, auth.c_str());
    headers = curl_slist_append(headers, "Content-Type: application/json");

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, payload.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 5L);
    curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 3L);

    CURLcode result = curl_easy_perform(curl);

    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);

    return result == CURLE_OK;
}


std::string HomeAssistantClient::getState(
    const std::string& entityId
)
{
    std::string response = getRawJson(entityId);

    if(response.empty())
        return "";

    try
    {
        json data =
            json::parse(response);


        if(!data.contains("state"))
        {
            std::cout
                << "Missing state for "
                << entityId
                << std::endl;

            return "";
        }


        return data["state"]
            .get<std::string>();

    }
    catch(const std::exception& e)
    {
        std::cout
            << "JSON parse error: "
            << e.what()
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
    std::string value =
        getState(entityId);


    if(value.empty())
        return 0.0f;


    if(value == "unknown" ||
       value == "unavailable")
    {
        return 0.0f;
    }


    try
    {
        return std::stof(value);
    }
    catch(const std::exception& e)
    {
        std::cout
            << "Float conversion failed for "
            << entityId
            << " value: "
            << value
            << std::endl;

        return 0.0f;
    }
}




int HomeAssistantClient::getIntState(
    const std::string& entityId
)
{
    std::string value =
        getState(entityId);


    if(value.empty())
        return 0;


    if(value == "unknown" ||
       value == "unavailable")
    {
        return 0;
    }


    try
    {
        return std::stoi(value);
    }
    catch(const std::exception& e)
    {
        std::cout
            << "Int conversion failed for "
            << entityId
            << " value: "
            << value
            << std::endl;

        return 0;
    }
}
