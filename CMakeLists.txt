cmake_minimum_required(VERSION 3.1.0)
project(qhttpengine VERSION 0.1.0 LANGUAGES CXX)

option(BUILD_DOC "Build Doxygen documentation" OFF)
option(BUILD_TESTS "Build the test suite" OFF)

set(PROJECT_NAME "QHttpEngine")
set(PROJECT_DESCRIPTION "Simple and secure HTTP server for Qt applications")
set(PROJECT_URL "https://github.com/nitroshare/qhttpengine")

find_package(Qt5Network 5.1 REQUIRED)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)

add_subdirectory(src)

if(BUILD_DOC)
    add_subdirectory(doc)
endif()

if(BUILD_TESTS)
    add_subdirectory(tests)
endif()
