cmake_minimum_required(VERSION 4.3)
project(Chip8QTEmulator)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)


if(APPLE)
    list(APPEND CMAKE_PREFIX_PATH "/opt/homebrew/opt/qt")
endif()

find_package(Qt6 REQUIRED COMPONENTS Core GUI Widgets)

add_executable(Chip8QTEmulator main.cpp
        emulator/chip8.h
        emulator/chip8.cpp)
target_link_libraries(Chip8QTEmulator
        Qt::Core
        Qt::Gui
        Qt::Widgets
)

