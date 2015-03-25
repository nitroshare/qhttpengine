find_package(Qt5Test REQUIRED)

enable_testing()

set(TESTS
    TestQHttpRequest
    TestQHttpResponse
    TestQHttpServer
)

foreach(TEST ${TESTS})
    add_executable(${TEST} "${TEST}.cpp")
    target_link_libraries(${TEST} Qt5::Test qhttpengine)
    add_test(NAME ${TEST} COMMAND ${TEST})
endforeach()
