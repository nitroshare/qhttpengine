## QHTTPEngine

[![Build Status](https://travis-ci.org/nitroshare/qhttpengine.svg?branch=master)](https://travis-ci.org/nitroshare/qhttpengine)
[![MIT License](http://img.shields.io/badge/license-MIT-blue.svg?style=flat)](http://opensource.org/licenses/MIT)

Simple set of classes for developing HTTP server applications in Qt.

### Build Requirements

QHttpEngine requires a modern C++ compiler supported by the Qt framework. Some examples include:

- Microsoft Visual C++ Express
- GCC (including MinGW-w64)
- Clang

CMake 2.8.11+ and Qt 5.1+ are required to build the library.

### Build Instructions

Use the instructions below to build the library:

1. Open a terminal or command prompt and run the following commands to create a directory for the files that will be built:

        mkdir build
        cd build

2. Run CMake to generate the Makefile that will be used to build the library:

        cmake ..

   **Note:** on Windows, you will need to change the last command to the following in order to generate a Makefile:

        cmake -G "NMake Makefiles" ..

3. Build the library:

   - **Unix-based (including MinGW-w64):**
            `make`
   - **Windows (MSVC++):**
            `nmake`
