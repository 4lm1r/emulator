cmake_minimum_required(VERSION 3.10)
project(Emulator)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -g") # Add -g flag here

find_package(Curses REQUIRED)
include_directories(${CURSES_INCLUDE_DIR} include)

set(SOURCES
    src/Screen.cpp
    src/Registers.cpp
    src/Memory.cpp
    src/CPU.cpp
    src/Emulator.cpp
    src/main.cpp
    src/CommandHandler.cpp
)

add_executable(emulator ${SOURCES})
target_link_libraries(emulator ${CURSES_LIBRARIES})
