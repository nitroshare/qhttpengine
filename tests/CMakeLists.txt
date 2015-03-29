find_package(Qt5Test REQUIRED)

set(TESTS
    TestQHttpSocket
)

foreach(TEST ${TESTS})
    add_executable(${TEST} "${TEST}.cpp")
    qt5_use_modules(${TEST} Test)
    target_link_libraries(${TEST} qhttpengine)

    add_test(NAME ${TEST} COMMAND ${TEST})
endforeach()
