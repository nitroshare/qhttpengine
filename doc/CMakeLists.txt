configure_file(Doxyfile.in "${CMAKE_CURRENT_BINARY_DIR}/Doxyfile")
add_custom_target(doc doxygen "${CMAKE_CURRENT_BINARY_DIR}/Doxyfile")
