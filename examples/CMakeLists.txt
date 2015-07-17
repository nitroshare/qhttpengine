set(EXAMPLES_INSTALL_DIR "${LIB_INSTALL_DIR}/${PROJECT_NAME}/examples")

set(EXAMPLES
    chatserver
    fileserver
)

foreach(EXAMPLE ${EXAMPLES})
    set(EXAMPLE_DIR "${EXAMPLES_INSTALL_DIR}/${EXAMPLE}")
    add_subdirectory(${EXAMPLE})
    install(DIRECTORY ${EXAMPLE}
        DESTINATION "${EXAMPLES_INSTALL_DIR}"
    )
endforeach()
