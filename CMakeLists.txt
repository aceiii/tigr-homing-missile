cmake_minimum_required(VERSION 3.15)

set(PROJECT_NAME tigr-test)

project(${PROJECT_NAME})

set(CMAKE_CXX_STANDARD 17)

set(SOURCE_FILES
    main.cpp
    vec2.cpp
    missiles.cpp
    random.cpp
)

add_executable(${PROJECT_NAME} ${SOURCE_FILES})

find_package(raylib CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)

target_link_libraries(${PROJECT_NAME} raylib spdlog::spdlog)
