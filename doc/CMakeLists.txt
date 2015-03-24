configure_file(Doxyfile.in "${CMAKE_CURRENT_BINARY_DIR}/Doxyfile")

add_custom_target(doc ALL
    doxygen "${CMAKE_CURRENT_BINARY_DIR}/Doxyfile"
)

install(DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/html"
    DESTINATION share/doc/qhttpengine
)
