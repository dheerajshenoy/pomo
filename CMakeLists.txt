cmake_minimum_required(VERSION 3.16)

project(pomo VERSION 1.0 LANGUAGES CXX)

find_package(Qt6 COMPONENTS Core Widgets REQUIRED)

# Output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/bin/Debug)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/bin/Release)

# Prefix
if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
  set(CMAKE_INSTALL_PREFIX "/usr" CACHE PATH "Install path prefix" FORCE)
endif()

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(${PROJECT_NAME}
    src/main.cpp
    src/Pomo.cpp
    src/miniaudio.cpp

    src/BlinkingLabel.hpp
    src/Notification.hpp
)

target_link_libraries(${PROJECT_NAME}
    Qt6::Core
    Qt6::Widgets
    pthread
    dl
    m
)

# Install target
install(TARGETS ${PROJECT_NAME}
    RUNTIME DESTINATION bin
)

# Optional: install config example
install(FILES config.toml.example DESTINATION share/pomo)

# Desktop file
install(FILES pomo.desktop DESTINATION share/applications)

# Optional: output a message about the build type
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
