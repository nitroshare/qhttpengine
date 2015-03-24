cmake_minimum_required(VERSION 2.8.9)
project(qhttpengine VERSION 0.1.0 LANGUAGES CXX)

set(PROJECT_NAME "QHttpEngine")
set(PROJECT_DESCRIPTION "Simple and secure HTTP server for Qt applications")
set(PROJECT_URL "https://github.com/nitroshare/qhttpengine")

find_package(Qt5Network 5.1 REQUIRED)
find_package(Qt5Test 5.1 REQUIRED)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)

enable_testing()

add_subdirectory(src)
add_subdirectory(tests)
