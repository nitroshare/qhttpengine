find_package(Qt5Test REQUIRED)

set(TESTS
    TestQHttpEngine
    TestQHttpRequest
    TestQHttpResponse
    TestQHttpServer
)

foreach(TEST ${TESTS})
    add_executable(${TEST} "${TEST}.cpp")
    qt5_use_modules(${TEST} Test)
    target_link_libraries(${TEST} qhttpengine)
    set_target_properties(${TEST} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}"
    )

    add_test(NAME ${TEST}
        COMMAND ${TEST}
    )
endforeach()
