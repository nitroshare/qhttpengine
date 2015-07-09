find_package(Qt5Widgets 5.1)

set(EXAMPLES_INSTALL_DIR lib/qhttpengine/examples)

add_subdirectory(chat)

if(Qt5Widgets_FOUND)
    add_subdirectory(fileserver)
else()
    message(WARNING "Qt5Widgets not found, GUI examples will not be built")
endif()
