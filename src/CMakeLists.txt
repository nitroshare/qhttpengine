configure_file(qhttpengine.h.in "${CMAKE_CURRENT_BINARY_DIR}/qhttpengine.h")
configure_file(qhttpengine.pc.in "${CMAKE_CURRENT_BINARY_DIR}/qhttpengine.pc" @ONLY)

set(HEADERS
    "${CMAKE_CURRENT_BINARY_DIR}/qhttpengine.h"
    QFilesystemHandler
    qfilesystemhandler.h
    QHttpHandler
    qhttphandler.h
    QHttpParser
    qhttpparser.h
    QHttpServer
    qhttpserver.h
    QHttpSocket
    qhttpsocket.h
    QIByteArray
    qibytearray.h
    QIODeviceCopier
    qiodevicecopier.h
    QLocalFile
    qlocalfile.h
    QObjectHandler
    qobjecthandler.h
)

set(SRC
    qfilesystemhandler.cpp
    qhttphandler.cpp
    qhttpparser.cpp
    qhttpserver.cpp
    qhttpsocket.cpp
    qibytearray.cpp
    qiodevicecopier.cpp
    qlocalfile.cpp
    qobjecthandler.cpp
)

if(WIN32)
    set(OUTPUT_NAME ${PROJECT_NAME}${PROJECT_VERSION_MAJOR})
    configure_file(resource.rc.in "${CMAKE_CURRENT_BINARY_DIR}/resource.rc")
    set(SRC ${SRC} "${CMAKE_CURRENT_BINARY_DIR}/resource.rc")
else()
    set(OUTPUT_NAME ${PROJECT_NAME})
endif()

add_library(qhttpengine SHARED ${SRC})
qt5_use_modules(qhttpengine Network)

target_include_directories(qhttpengine PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}"
    "${CMAKE_CURRENT_BINARY_DIR}"
)

set_target_properties(qhttpengine PROPERTIES
    DEFINE_SYMBOL QT_NO_SIGNALS_SLOTS_KEYWORDS
    DEFINE_SYMBOL QHTTPENGINE_LIBRARY
    PUBLIC_HEADER "${HEADERS}"
    OUTPUT_NAME ${OUTPUT_NAME}
    VERSION ${PROJECT_VERSION}
    SOVERSION ${PROJECT_VERSION_MAJOR}
    LIBRARY_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}"
    RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}"
)

install(TARGETS qhttpengine
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION "${LIB_INSTALL_DIR}"
    ARCHIVE DESTINATION "${LIB_INSTALL_DIR}"
    PUBLIC_HEADER DESTINATION include/qhttpengine
)

install(FILES "${CMAKE_CURRENT_BINARY_DIR}/qhttpengine.pc"
    DESTINATION lib/pkgconfig
)
