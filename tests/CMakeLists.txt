find_package(Qt5Test 5.1 REQUIRED)

add_subdirectory(common)

set(TESTS
    TestQFilesystemHandler
    TestQHttpBasicAuth
    TestQHttpHandler
    TestQHttpMiddleware
    TestQHttpParser
    TestQHttpRange
    TestQHttpServer
    TestQHttpSocket
    TestQIByteArray
    TestQIODeviceCopier
    TestQLocalAuth
    TestQLocalFile
    TestQObjectHandler
)

foreach(TEST ${TESTS})
    add_executable(${TEST} ${TEST}.cpp)
    set_target_properties(${TEST} PROPERTIES
        CXX_STANDARD 11
        CXX_STANDARD_REQUIRED ON
    )
    target_link_libraries(${TEST} Qt5::Test QHttpEngine common)
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
