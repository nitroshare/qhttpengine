configure_file(config.h.in "${CMAKE_CURRENT_BINARY_DIR}/config.h")
configure_file(qhttpengine.pc.in "${CMAKE_CURRENT_BINARY_DIR}/qhttpengine.pc" @ONLY)

set(INCLUDE
    "${CMAKE_CURRENT_BINARY_DIR}/config.h"
    qhttprequest.h
    qhttpsocket.h
    qjsonserver.h
)

set(SRC
    qhttprequest.cpp
    qhttpsocket.cpp
    qjsonserver.cpp
)

add_library(qhttpengine SHARED ${SRC})
qt5_use_modules(qhttpengine Network)

target_include_directories(qhttpengine PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}"
    "${CMAKE_CURRENT_BINARY_DIR}"
)

set_target_properties(qhttpengine PROPERTIES
    DEFINE_SYMBOL QHTTPENGINE_LIBRARY
    VERSION ${PROJECT_VERSION}
    SOVERSION ${PROJECT_VERSION_MAJOR}
    LIBRARY_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}"
    RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}"
)

install(TARGETS qhttpengine
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)

install(FILES ${INCLUDE}
    DESTINATION include/qhttpengine
)
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/qhttpengine.pc"
    DESTINATION lib/pkgconfig
)
