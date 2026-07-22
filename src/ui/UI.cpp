#include "UI.h"
#include "../services/HomeAssistantClient.h"
#include "../config/secrets.h"
#include <iostream>
#include "../ui/screens/HomeScreen.h"


static HomeAssistantClient ha(
    HA_URL,
    HA_TOKEN
);

static HomeScreen homeScreen;

void UI::init()
{

    homeScreen.create();


    temperature.create();


    temperature.setTemperature(
        ha.getFloatState(
            "sensor.camera_temperatura_camera"
        )
    );


    temperature.setHumidity(
        ha.getFloatState(
            "sensor.camera_umidita_camera"
        )
    );

}

void UI::update()
{

    std::cout
        << "Updating Home Assistant values"
        << std::endl;


    float temp =
        ha.getFloatState(
            "sensor.camera_temperatura_camera"
        );


    float hum =
        ha.getFloatState(
            "sensor.camera_umidita_camera"
        );


    std::cout
        << "Temperature: "
        << temp
        << " C"
        << std::endl;


    std::cout
        << "Humidity: "
        << hum
        << " %"
        << std::endl;


    temperature.setTemperature(
        temp
    );


    temperature.setHumidity(
        hum
    );
    lv_refr_now(nullptr);
}