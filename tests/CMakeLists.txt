find_package(Qt5Test REQUIRED)

add_subdirectory(common)

set(TESTS
    TestQFilesystemHandler.cpp
    TestQHttpHandler.cpp
    TestQHttpParser.cpp
    TestQHttpServer.cpp
    TestQHttpSocket.cpp
    TestQIByteArray.cpp
    TestQIODeviceCopier.cpp
    TestQLocalFile.cpp
    TestQObjectHandler.cpp
)

foreach(TEST ${TESTS})
    get_filename_component(NAME "${TEST}" NAME_WE)

    add_executable(${NAME} "${TEST}")
    qt5_use_modules(${NAME} Test)
    target_link_libraries(${NAME} QHttpEngine common)
    set_target_properties(${NAME} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}"
    )

    add_test(NAME ${NAME}
        COMMAND ${NAME}
    )
endforeach()
