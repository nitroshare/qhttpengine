configure_file(config.h.in "${CMAKE_CURRENT_BINARY_DIR}/config.h")
configure_file(qhttpengine.pc.in "${CMAKE_CURRENT_BINARY_DIR}/qhttpengine.pc" @ONLY)

set(INCLUDE
    "${CMAKE_CURRENT_BINARY_DIR}/config.h"
    qhttprequest.h
    qhttpresponse.h
    qhttpserver.h
)

set(SRC
    qhttprequest.cpp
    qhttpresponse.cpp
    qhttpserver.cpp
)

add_library(qhttpengine SHARED ${SRC})
target_link_libraries(qhttpengine Qt5::Network)

set_target_properties(qhttpengine PROPERTIES
    DEFINE_SYMBOL QHTTPENGINE_LIBRARY
    VERSION ${PROJECT_VERSION}
    SOVERSION ${PROJECT_VERSION_MAJOR}
)

install(TARGETS qhttpengine
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)

install(FILES ${INCLUDE}
    DESTINATION include/qhttpengine)
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/qhttpengine.pc"
    DESTINATION lib/pkgconfig
)
