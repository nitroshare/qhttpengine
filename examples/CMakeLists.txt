find_package(Qt5Gui 5.1)

set(EXAMPLES_INSTALL_DIR lib/qhttpengine/examples)

if(Qt5Gui_FOUND)
    add_subdirectory(fileserver)
endif()
