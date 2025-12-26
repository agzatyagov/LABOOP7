cmake_minimum_required(VERSION 3.20)
project(Lab_OOP7 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(core
    src/dungeon.cpp
    src/npc.cpp
    src/npc_factory.cpp
    src/observer.cpp
    src/visitor.cpp
)

target_include_directories(core PUBLIC include)

add_executable(Lab_OOP7 main.cpp)
target_link_libraries(Lab_OOP7 PRIVATE core)

include(CTest)

if(BUILD_TESTING)
    include(FetchContent)
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

    FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
    )
    FetchContent_MakeAvailable(googletest)

    add_executable(tests tests/tests.cpp)
    target_link_libraries(tests PRIVATE core gtest_main)
    target_include_directories(tests PRIVATE include)

    include(GoogleTest)
    gtest_discover_tests(tests)
endif()

