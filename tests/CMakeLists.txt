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
        WORKING_DIRECTORY "$<TARGET_FILE_DIR:QHttpEngine>"
    )
endforeach()
