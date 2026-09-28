cmake_minimum_required(VERSION 4.3)
project(Chip8QTEmulator)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)



if(APPLE)
    list(APPEND CMAKE_PREFIX_PATH "/opt/homebrew/opt/qt")
    set(MACOSX_BUNDLE_ICON_FILE chip8.icns)
    set(APP_ICON ${CMAKE_SOURCE_DIR}/assets/icons/chip8.icns)
    set_source_files_properties(${APP_ICON} PROPERTIES MACOSX_PACKAGE_LOCATION "Resources")
endif()

find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets Svg Multimedia)

add_executable(Chip8QTEmulator main.cpp
        emulator/Chip8/Chip8.h
        emulator/Chip8/Chip8.cpp
        emulator/Exception/IllegalInstructionException.h
        emulator/Exception/BadValueException.h
        emulator/Exception/ROMLoadingException.h
        emulator/Chip8/Chip8Display.cpp
        emulator/Chip8/Chip8Display.h
        assets/icons.qrc
        emulator/Chip8/Chip8Controller.cpp
        emulator/Chip8/Chip8Controller.h
        ui/mainwindow.h ui/mainwindow.cpp ui/mainwindow.ui
        emulator/Logging/LogHandler.h
        emulator/Chip8/Chip8Sound.cpp
        emulator/Chip8/Chip8Sound.h
)
set(AUTOGEN_INCLUDE ${CMAKE_CURRENT_BINARY_DIR}/Chip8QTEmulator_autogen/include)
file(MAKE_DIRECTORY ${AUTOGEN_INCLUDE})

target_include_directories(Chip8QTEmulator PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/emulator
        ${AUTOGEN_INCLUDE}
)
target_link_libraries(Chip8QTEmulator PRIVATE Qt6::Core Qt6::Gui Qt6::Widgets Qt6::Svg Qt6::Multimedia)

include(GNUInstallDirs)
install(TARGETS Chip8QTEmulator
        BUNDLE DESTINATION .
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(Chip8QTEmulator)
endif()
