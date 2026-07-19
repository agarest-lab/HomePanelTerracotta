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

)

target_include_directories(HomePanel
    PRIVATE
    include
)

target_link_libraries(HomePanel
    PRIVATE
    lvgl
    SDL3::SDL3
)