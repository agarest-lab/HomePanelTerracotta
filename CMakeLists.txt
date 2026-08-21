cmake_minimum_required(VERSION 3.20)

project(HomePanel)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(LV_CONF_PATH ${CMAKE_CURRENT_SOURCE_DIR}/lv_conf.h)

set(LV_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(CONFIG_LV_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(CONFIG_LV_BUILD_DEMOS OFF CACHE BOOL "" FORCE)
set(LV_BUILD_DEMOS OFF CACHE BOOL "" FORCE)
# LVGL
add_subdirectory(external/lvgl)

# SDL
set(SDL_PULSEAUDIO OFF CACHE BOOL "" FORCE)
add_subdirectory(external/SDL)

# HomePanel
add_executable(HomePanel
    src/main.cpp
    src/app/Application.cpp
    src/graphics/Display.cpp
    src/graphics/SDLDisplay.cpp
    src/ui/UI.cpp
    src/ui/screens/HomeScreen.cpp
    src/ui/widgets/ClockCard.cpp
    src/ui/widgets/CameraCard.cpp
    src/ui/widgets/StatusCardsRow.cpp
    src/services/HomeAssistantClient.cpp
    src/services/HomeAssistantRepository.cpp
    src/services/CameraFeed.cpp
    src/assets/montserrat_orario_140.c
    src/assets/montserrat_orario_140.c
    src/assets/weather_icons/sun_96.c
    src/assets/weather_icons/moon_96.c
    src/assets/weather_icons/rain_96.c
    src/assets/weather_icons/storm_96.c
    src/assets/weather_icons/sun_96.c
    src/assets/weather_icons/wind_96.c
    src/assets/weather_icons/moon_cloud_96.c
    src/assets/weather_icons/snow_96.c
    src/assets/weather_icons/sun_cloud_96.c
    src/ui/widgets/WeatherCardsRow.cpp
    src/services/CalendarFeed.cpp
    src/ui/widgets/CalendarCard.cpp
    src/ui/widgets/EnergyGaugesPanel.h
    src/ui/widgets/ToggleRow.cpp
    src/ui/widgets/ToggleRow.h
    src/ui/widgets/EnergyGaugesPanel.cpp
    src/ui/widgets/DayNightCardsRow.cpp
    src/ui/widgets/WeatherIconCard.cpp
    src/ui/widgets/CarStatusCard.cpp
    src/ui/widgets/MotionLedsRow.cpp
    src/ui/widgets/WashingMachineCard.cpp
)

find_package(CURL REQUIRED)
find_package(Threads REQUIRED)
find_package(JPEG REQUIRED)

target_include_directories(HomePanel
    PRIVATE
    include
)

target_link_libraries(HomePanel
    lvgl
    SDL3::SDL3
    CURL::libcurl
    Threads::Threads
    JPEG::JPEG
)
