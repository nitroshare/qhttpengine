# Prepare the config and .pc files
configure_file(config.h.in "${CMAKE_CURRENT_BINARY_DIR}/config.h")
configure_file(qhttpengine.pc.in "${CMAKE_CURRENT_BINARY_DIR}/qhttpengine.pc" @ONLY)

# Include files for the library
set(INCLUDE
    "${CMAKE_CURRENT_BINARY_DIR}/config.h"
    qhttprequest.h
    qhttpresponse.h
    qhttpserver.h
)

# Source files for the library
set(SRC
    qhttprequest.cpp
    qhttpresponse.cpp
    qhttpserver.cpp
)

# Create the shared library and link it against Qt5Network
add_library(qhttpengine SHARED ${SRC})
target_link_libraries(qhttpengine Qt5::Network)

# Set various library properties
set_target_properties(qhttpengine PROPERTIES
    DEFINE_SYMBOL QHTTPENGINE_LIBRARY
    VERSION ${PROJECT_VERSION}
    SOVERSION ${PROJECT_VERSION_MAJOR}
)

# Specify the location for the target
install(TARGETS qhttpengine
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)

# Specify the location to install include and .pc files
install(FILES ${INCLUDE}
    DESTINATION include/qhttpengine)
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/qhttpengine.pc"
    DESTINATION lib/pkgconfig
)
