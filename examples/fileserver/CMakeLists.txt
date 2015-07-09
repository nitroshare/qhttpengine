set(SRC
    main.cpp
)

add_executable(fileserver WIN32 ${SRC})
target_link_libraries(fileserver qhttpengine)

set_target_properties(fileserver PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}"
)

install(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
    DESTINATION "${EXAMPLES_INSTALL_DIR}"
)

install(TARGETS fileserver
    RUNTIME DESTINATION "${EXAMPLES_INSTALL_DIR}/fileserver"
)
