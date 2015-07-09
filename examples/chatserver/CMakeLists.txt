set(SRC
    apihandler.cpp
    main.cpp
)

qt5_add_resources(QRC resources.qrc)

add_executable(chatserver ${SRC} ${QRC})
target_link_libraries(chatserver qhttpengine)

set_target_properties(chatserver PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}"
)

install(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
    DESTINATION "${EXAMPLES_INSTALL_DIR}"
)

install(TARGETS chatserver
    RUNTIME DESTINATION "${EXAMPLES_INSTALL_DIR}/chatserver"
)
