find_package(Qt5Test REQUIRED)

add_subdirectory(common)

set(TESTS
    TestByteUtils
    TestQFilesystemHandler
    TestQHttpServer
    TestQHttpSocket
    TestQHttpSubHandler
    TestQIODeviceCopier
)

foreach(TEST ${TESTS})
    add_executable(${TEST} "${TEST}.cpp")
    qt5_use_modules(${TEST} Test)
    target_link_libraries(${TEST} qhttpengine common)
    set_target_properties(${TEST} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}"
    )

    add_test(NAME ${TEST}
        COMMAND ${TEST}
    )
endforeach()
