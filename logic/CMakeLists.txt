cmake_minimum_required(VERSION 3.10.0)
project(crocodile_back VERSION 0.1.0 LANGUAGES C CXX)


set(SOURCE_FILE
    ./src/GameState.cpp
    ./src/Player.cpp
    ./src/GameController.cpp
    ./src/ChatManager.cpp
    )

include_directories(${CMAKE_SOURCE_DIR}/include)

# add_executable(crocodile_back ${SOURCE_FILE})

add_library(crocodile_back SHARED ${SOURCE_FILE})

set_target_properties(crocodile_back PROPERTIES PREFIX "")