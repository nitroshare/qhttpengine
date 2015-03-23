# CMake 3.1 was the first release to provide target_compile_features
cmake_minimum_required(VERSION 3.1.0)
project(qhttpengine VERSION 0.1.0 LANGUAGES CXX)

set(PROJECT_NAME "QHttpEngine")
set(PROJECT_DESCRIPTION "Simple and secure HTTP server for Qt applications")
set(PROJECT_URL "https://github.com/nitroshare/qhttpengine")

# Ensure that the required Qt libraries are present
find_package(Qt5Network 5.1 REQUIRED)

# Add the build directory to the include path and automatically run MOC
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)

# Add the source directory
add_subdirectory(src)
