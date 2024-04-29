cmake_minimum_required(VERSION 3.15)

set(PROJECT_NAME tigr-test)

project(${PROJECT_NAME})

set(CMAKE_CXX_STANDARD 14)

set(SOURCE_FILES
    main.cpp
    tigr.h tigr.c
    vec2.h vec2.cpp
    missiles.h missiles.cpp
    random.h random.cpp)

add_executable(${PROJECT_NAME} ${SOURCE_FILES})

if(APPLE)
    find_library(COCOA_LIBRARY Cocoa)
    find_library(OPENGL_LIBRARY Opengl)

    set(LIBS ${LIBS} ${COCOA_LIBRARY} ${OPENGL_LIBRARY})

    #set_target_properties(${PROJECT_NAME} PROPERTIES
    #    MACOSX_BUNDLE TRUE)
endif()

target_link_libraries(${PROJECT_NAME} ${LIBS})
