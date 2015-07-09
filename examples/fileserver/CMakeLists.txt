set(INCLUDE
    mainwindow.h
)

set(SRC
    main.cpp
    mainwindow.cpp
)

set(OTHER
    CMakeLists.txt
    mainwindow.ui
)

qt5_wrap_ui(UI
    mainwindow.ui
)

add_executable(fileserver WIN32 ${SRC} ${UI})
qt5_use_modules(fileserver Widgets)
target_link_libraries(fileserver qhttpengine)

set_target_properties(fileserver PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}"
)

install(TARGETS fileserver
    RUNTIME DESTINATION ${EXAMPLES_INSTALL_DIR}/fileserver
)

install(FILES ${INCLUDE} ${SRC} ${OTHER}
    DESTINATION ${EXAMPLES_INSTALL_DIR}/fileserver
)
