set(SRC
    main.cpp
)

add_executable(fileserver ${SRC})
target_link_libraries(fileserver qhttpengine)
set_target_properties(fileserver PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}"
)

install(TARGETS fileserver
    RUNTIME DESTINATION "${EXAMPLE_DIR}"
)
