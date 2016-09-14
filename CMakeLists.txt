cmake_minimum_required(VERSION 2.8)
project(Thing C CXX)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/cmake)
include(Tools3DS)

find_package(CTRULIB REQUIRED)

file(GLOB_RECURSE SOURCE_FILES
    src/*
)
include_directories(include)

add_executable(Detour ${SOURCE_FILES})
target_link_libraries(Detour 3ds::ctrulib)

add_cia_target(Detour ${CMAKE_CURRENT_LIST_DIR}/meta/detour.rsf ${CMAKE_CURRENT_LIST_DIR}/meta/banner.png ${CMAKE_CURRENT_LIST_DIR}/meta/audio.wav)
