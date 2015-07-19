find_package(Qt5Test 5.1 REQUIRED)

add_subdirectory(common)

set(TESTS
    TestQFilesystemHandler
    TestQHttpHandler
    TestQHttpParser
    TestQHttpServer
    TestQHttpSocket
    TestQIByteArray
    TestQIODeviceCopier
    TestQLocalFile
    TestQObjectHandler
)

foreach(TEST ${TESTS})
    add_executable(${TEST} ${TEST}.cpp)
    qt5_use_modules(${TEST} Test)
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
