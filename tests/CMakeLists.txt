find_package(Qt5Test REQUIRED)

add_subdirectory(common)

set(TESTS
    core/TestQHttpServer.cpp
    core/TestQHttpSocket.cpp
    handler/TestQFilesystemHandler.cpp
    handler/TestQHttpSubHandler.cpp
    handler/TestQObjectHandler.cpp
    util/TestQHttpParser.cpp
    util/TestQIByteArray.cpp
    util/TestQIODeviceCopier.cpp
)

foreach(TEST ${TESTS})
    get_filename_component(NAME "${TEST}" NAME_WE)

    add_executable(${NAME} "${TEST}")
    qt5_use_modules(${NAME} Test)
    target_link_libraries(${NAME} qhttpengine common)
    set_target_properties(${NAME} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}"
    )

    add_test(NAME ${NAME}
        COMMAND ${NAME}
    )
endforeach()
