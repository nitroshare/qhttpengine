set(EXAMPLES_INSTALL_DIR "${LIB_INSTALL_DIR}/qhttpengine/examples")
set(EXAMPLES_BIN_DIR "${EXAMPLES_INSTALL_DIR}/bin")

set(EXAMPLES
    chatserver
    fileserver
)

foreach(EXAMPLE ${EXAMPLES})
    add_subdirectory(${EXAMPLE})
    install(DIRECTORY ${EXAMPLE}
        DESTINATION "${EXAMPLES_INSTALL_DIR}"
    )
endforeach()
