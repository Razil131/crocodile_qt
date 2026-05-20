cmake_minimum_required(VERSION 3.10.0)
project(crocodile_back VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(SOURCE_FILE
    ./src/GameState.cpp
    ./src/Player.cpp
    ./src/GameController.cpp
    ./src/ChatManager.cpp
    ./src/RoundManager.cpp
    ./src/WordManager.cpp
    )

include_directories(${CMAKE_SOURCE_DIR}/include)

# add_executable(crocodile_back ${SOURCE_FILE})

add_library(crocodile_back SHARED ${SOURCE_FILE})

set_target_properties(crocodile_back PROPERTIES PREFIX "")

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
)

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE) 
FetchContent_MakeAvailable(googletest)

add_executable(run_project_tests 
    ./tests/GameController_test.cpp
)

target_include_directories(run_project_tests PRIVATE ${CMAKE_SOURCE_DIR}/include)

target_link_libraries(run_project_tests 
    PRIVATE 
    crocodile_back 
    gtest_main
)

enable_testing()
include(GoogleTest)
gtest_discover_tests(run_project_tests)
