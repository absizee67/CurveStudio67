cmake_minimum_required(VERSION 3.16)
project(CurveStudio LANGUAGES C CXX)

if(POLICY CMP0135)
    cmake_policy(SET CMP0135 NEW)
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(raylib 5.5 QUIET)
if(NOT TARGET raylib)
    include(FetchContent)
    set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
    FetchContent_Declare(raylib
        URL https://github.com/raysan5/raylib/archive/refs/tags/5.5.tar.gz
    )
    FetchContent_MakeAvailable(raylib)
endif()

add_executable(CurveStudio
    Application/main.cpp
    Interface/MainWindow.cpp
    Interface/MainWindow.h
    Interface/Scene.cpp
    Interface/Scene.h
)

target_link_libraries(CurveStudio PRIVATE raylib)
if(MSVC)
    target_compile_options(CurveStudio PRIVATE /utf-8)
endif()
