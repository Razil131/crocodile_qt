cmake_minimum_required(VERSION 3.16)

project(crocodile_back VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)

find_package(Qt6 REQUIRED COMPONENTS Core Gui Network Test)

set(SOURCE_FILE
    ./src/GameState.cpp
    ./src/Player.cpp
    ./src/GameController.cpp
    ./src/ChatManager.cpp
    ./src/RoundManager.cpp
    ./src/WordManager.cpp
    ./src/RoundController.cpp
    ./src/ChatController.cpp
    ./src/PlayerController.cpp
    ./src/NetworkManager.cpp

    ./include/DrawController.hpp
    ./include/GameController.hpp
    ./include/exportMacro.hpp
    ./include/drawCommand.hpp
    ./include/RoundController.hpp
    ./include/ChatController.hpp
    ./include/PlayerController.hpp
    ./include/NetworkTypes.hpp
    ./include/NetworkManager.hpp
)

include_directories(${CMAKE_SOURCE_DIR}/include)

add_library(crocodile_back SHARED ${SOURCE_FILE})

target_compile_definitions(crocodile_back PRIVATE CROCODILE_BACK_LIBRARY)

set_target_properties(crocodile_back PROPERTIES PREFIX "")

target_link_libraries(crocodile_back
    PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Network
)

return()

include(FetchContent)

FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
)

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(googletest)

set(TEST_SOURCES
    ./tests/GameState_test.cpp
    ./tests/ChatManager_test.cpp
    ./tests/WordManager_test.cpp
    ./tests/Player_test.cpp
    ./tests/PlayerController_test.cpp
    ./tests/ChatController_test.cpp
    ./tests/RoundManager_test.cpp
    ./tests/RoundController_test.cpp
    ./tests/GameController_test.cpp
)

add_executable(run_project_tests
    ${TEST_SOURCES}
)

target_compile_definitions(run_project_tests
    PRIVATE UNIT_TESTS
)

target_include_directories(run_project_tests
    PRIVATE
    ${CMAKE_SOURCE_DIR}/include
)

target_link_libraries(run_project_tests
    PRIVATE
    crocodile_back
    gtest_main
    gmock
    Qt6::Core
    Qt6::Gui
    Qt6::Test
    Qt6::Network
)

enable_testing()
include(GoogleTest)
gtest_discover_tests(run_project_tests)