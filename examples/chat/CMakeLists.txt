set(SRC
    main.cpp
)

add_executable(chat ${SRC})
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
