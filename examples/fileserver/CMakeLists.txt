set(SRC
    main.cpp
)

add_executable(fileserver WIN32 ${SRC})
qt5_use_modules(fileserver Widgets)
target_link_libraries(fileserver qhttpengine)

set_target_properties(fileserver PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}"
)

install(TARGETS fileserver
    RUNTIME DESTINATION ${EXAMPLES_INSTALL_DIR}/fileserver
)

install(FILES ${SRC}
    DESTINATION ${EXAMPLES_INSTALL_DIR}/fileserver
)
