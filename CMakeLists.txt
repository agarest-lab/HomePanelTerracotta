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
    src/ui/widgets/EnergyCard.cpp
    src/ui/widgets/EntityButtonRow.cpp
    src/assets/energy_bg.c
    src/assets/montserrat_orario_140.c
    src/ui/widgets/WeatherCardsRow.cpp
    src/services/CalendarFeed.cpp
    src/ui/widgets/CalendarCard.cpp
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
