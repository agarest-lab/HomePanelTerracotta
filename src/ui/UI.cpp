#include "UI.h"
#include "../services/HomeAssistantRepository.h"
#include "../ui/screens/HomeScreen.h"

#include <iostream>


static HomeScreen homeScreen;


void UI::init()
{

    homeScreen.create();


    temperature.create();


    auto& repo = HomeAssistantRepository::instance();

    // Stesse entità già osservate da HomeScreen (camera = "notte"):
    // watch() è idempotente, quindi non c'è problema a richiamarlo qui.
    repo.watch("sensor.camera_temperatura_camera");
    repo.watch("sensor.camera_umidita_camera");


    temperature.setTemperature(
        repo.getFloatState("sensor.camera_temperatura_camera")
    );


    temperature.setHumidity(
        repo.getFloatState("sensor.camera_umidita_camera")
    );

}


void UI::update()
{

    auto& repo = HomeAssistantRepository::instance();


    float temp = repo.getFloatState("sensor.camera_temperatura_camera");
    float hum  = repo.getFloatState("sensor.camera_umidita_camera");


    temperature.setTemperature(temp);
    temperature.setHumidity(hum);

    lv_refr_now(nullptr);
}
