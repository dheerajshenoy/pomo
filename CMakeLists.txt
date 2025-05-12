cmake_minimum_required(VERSION 3.16)

project(Pomo VERSION 1.0 LANGUAGES CXX)

# Choose Qt version: adjust if needed
find_package(Qt6 COMPONENTS Core Widgets REQUIRED)
# If you're using Qt5 instead:
# find_package(Qt5 COMPONENTS Widgets REQUIRED)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(MyQtApp
    src/main.cpp
    src/Pomo.cpp
    src/miniaudio.cpp
)

target_link_libraries(MyQtApp
    Qt6::Core
    Qt6::Widgets
    pthread
    dl
    m
)

