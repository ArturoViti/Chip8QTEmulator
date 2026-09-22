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
        emulator/Chip8/Chip8.h
        emulator/Chip8/Chip8.cpp
        emulator/Exception/IllegalInstructionException.h
        emulator/Exception/BadValueException.h
)
target_link_libraries(Chip8QTEmulator
        Qt::Core
        Qt::Gui
        Qt::Widgets
)

target_include_directories(Chip8QTEmulator PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/emulator
)