cmake_minimum_required(VERSION 2.8.11)
project(qhttpengine)

if(NOT (CMAKE_MAJOR_VERSION VERSION_LESS 3))
    cmake_policy(SET CMP0043 OLD)
endif()

option(BUILD_DOC "Build Doxygen documentation" OFF)
option(BUILD_EXAMPLES "Build the example applications" OFF)
option(BUILD_TESTS "Build the test suite" OFF)

set(PROJECT_NAME "QHttpEngine")
set(PROJECT_DESCRIPTION "Simple and secure HTTP server for Qt applications")
set(PROJECT_AUTHOR "Nathan Osman")
set(PROJECT_URL "https://github.com/nitroshare/qhttpengine")

set(PROJECT_VERSION_MAJOR 0)
set(PROJECT_VERSION_MINOR 1)
set(PROJECT_VERSION_PATCH 0)
set(PROJECT_VERSION "${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH}")

find_package(Qt5Network 5.1 REQUIRED)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)

add_subdirectory(src)

if(BUILD_DOC)
    add_subdirectory(doc)
endif()

if(BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()

if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()
