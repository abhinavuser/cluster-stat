cmake_minimum_required(VERSION 3.0)

set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED True)

project(surveillant)

add_executable(
    surveillant
    main.cpp
)

add_library(lvgl OBJECT EXCLUDE_FROM_ALL)
file(GLOB_RECURSE lvgl_sources "libraries/lvgl/*.c")
target_sources(lvgl PRIVATE
    ${lvgl_sources}
)

#target_include_directories(lvgl SYSTEM PUBLIC libraries/lvgl/lvgl.h)
target_include_directories(lvgl SYSTEM PUBLIC libraries/lvgl/)
target_include_directories(lvgl SYSTEM PUBLIC libraries/lvgl/../)

add_library(lv_drivers OBJECT EXCLUDE_FROM_ALL)
file(GLOB_RECURSE lv_drivers_sources "libraries/lv_drivers/*.c")
target_sources(lv_drivers PRIVATE
    ${lv_drivers_sources}
)

target_link_libraries(lv_drivers PUBLIC lvgl)
target_include_directories(lv_drivers SYSTEM PUBLIC "libraries/lv_drivers/")

target_link_libraries(surveillant PRIVATE
    lvgl
    lv_drivers
    pthread
)