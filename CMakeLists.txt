cmake_minimum_required(VERSION 3.20)

project(HomePanel)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(LV_CONF_PATH ${CMAKE_CURRENT_SOURCE_DIR}/lv_conf.h)

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
    
    src/ui/widgets/TemperatureCard.cpp

    src/services/HomeAssistantClient.cpp

    src/ui/widgets/EnergyCard.cpp

)

find_package(CURL REQUIRED)

target_include_directories(HomePanel
    PRIVATE
    include
)

target_link_libraries(HomePanel
    lvgl
    SDL3::SDL3
    CURL::libcurl
)