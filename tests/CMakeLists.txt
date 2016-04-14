if (Qt4_FOUND)
    find_package(Qt4 4.8 REQUIRED QtTest)
else()
    find_package(Qt5Test 5.1 REQUIRED)
endif()

add_subdirectory(common)

set(TESTS
    TestQHttpParser
    TestQIByteArray
    TestQLocalFile
)

# These don't work with Qt4
if (NOT Qt4_FOUND)
    set(TESTS ${TESTS}
        TestQFilesystemHandler
        TestQHttpHandler
        TestQHttpServer
        TestQHttpSocket
        TestQIODeviceCopier
        TestQObjectHandler
    )
endif()

foreach(TEST ${TESTS})
    add_executable(${TEST} ${TEST}.cpp)
    
if (Qt4_FOUND)
    target_link_libraries(${TEST} Qt4::QtTest)   

    target_include_directories(${TEST} PRIVATE
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../src/mimetypes/include/QtMimeTypes>"
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../src/mimetypes/src/mimetypes/inqt5>"
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../src/qjson-backport>"
    )  
else()
    qt5_use_modules(${TEST} Test)
endif()

    target_link_libraries(${TEST} QHttpEngine common)
    
    add_test(NAME ${TEST}
        COMMAND ${TEST}
    )
endforeach()

# On Windows, the library's DLL must exist in the same directory as the test
# executables which link against it - create a custom command to copy it
if(WIN32 AND NOT BUILD_STATIC)
    add_custom_target(QHttpEngine-copy ALL
        "${CMAKE_COMMAND}" -E copy_if_different "$<TARGET_FILE:QHttpEngine>" "${CMAKE_CURRENT_BINARY_DIR}"
        DEPENDS QHttpEngine
    )
endif()
