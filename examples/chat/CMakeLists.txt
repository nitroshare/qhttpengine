set(SRC
    apihandler.cpp
    main.cpp
)

qt5_add_resources(QRC resources.qrc)

add_executable(chat ${SRC} ${QRC})
target_link_libraries(chat qhttpengine)

set_target_properties(chat PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}"
)

install(TARGETS chat
    RUNTIME DESTINATION ${EXAMPLES_INSTALL_DIR}/chat
)

install(FILES ${SRC}
    DESTINATION ${EXAMPLES_INSTALL_DIR}/chat
)
