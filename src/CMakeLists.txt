file(GLOB HEADERS "${PROJECT_NAME}/*")

configure_file(qhttpengine.h.in "${CMAKE_CURRENT_BINARY_DIR}/qhttpengine.h")
set(HEADERS "${HEADERS}" "${CMAKE_CURRENT_BINARY_DIR}/qhttpengine.h")

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
    
    qjson-backport/qjsonarray.cpp
    qjson-backport/qjson.cpp
    qjson-backport/qjsondocument.cpp
    qjson-backport/qjsonobject.cpp
    qjson-backport/qjsonparser.cpp
    qjson-backport/qjsonvalue.cpp
    qjson-backport/qjsonwriter.cpp
    
    mimetypes/src/mimetypes/qmimedatabase.cpp
    mimetypes/src/mimetypes/qmimeglobpattern.cpp
    mimetypes/src/mimetypes/qmimemagicrule.cpp
    mimetypes/src/mimetypes/qmimemagicrulematcher.cpp
    mimetypes/src/mimetypes/qmimeprovider.cpp
    mimetypes/src/mimetypes/qmimetype.cpp
    mimetypes/src/mimetypes/qmimetypeparser.cpp    
    mimetypes/src/mimetypes/inqt5/qstandardpaths.cpp
    mimetypes/src/mimetypes/inqt5/qstandardpaths_unix.cpp    
)

if(WIN32)
    configure_file(resource.rc.in "${CMAKE_CURRENT_BINARY_DIR}/resource.rc")
    set(SRC ${SRC} "${CMAKE_CURRENT_BINARY_DIR}/resource.rc")
endif()

if(BUILD_STATIC)
    add_library(QHttpEngine STATIC ${HEADERS} ${SRC})
else()
    add_library(QHttpEngine SHARED ${HEADERS} ${SRC})
endif()

target_link_libraries(QHttpEngine Qt4::QtCore Qt4::QtNetwork Qt4::QtGui)

target_include_directories(QHttpEngine PUBLIC
    "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>"
    "$<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>"
    "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/mimetypes/include/QtMimeTypes>"
    "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/mimetypes/src/mimetypes/inqt5>"
    "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/qjson-backport>"
    "$<INSTALL_INTERFACE:${INCLUDE_INSTALL_DIR}>"
)

set_target_properties(QHttpEngine PROPERTIES
    DEFINE_SYMBOL QT_NO_SIGNALS_SLOTS_KEYWORDS
    DEFINE_SYMBOL QHTTPENGINE_LIBRARY
    PUBLIC_HEADER "${HEADERS}"
    VERSION ${PROJECT_VERSION}
    SOVERSION ${PROJECT_VERSION_MAJOR}
)

install(TARGETS QHttpEngine EXPORT QHttpEngine-export
    RUNTIME DESTINATION "${BIN_INSTALL_DIR}"
    LIBRARY DESTINATION "${LIB_INSTALL_DIR}"
    ARCHIVE DESTINATION "${LIB_INSTALL_DIR}"
    PUBLIC_HEADER DESTINATION "${INCLUDE_INSTALL_DIR}/${PROJECT_NAME}"
)

install(EXPORT QHttpEngine-export DESTINATION "${CMAKECONFIG_INSTALL_DIR}"
    FILE ${PROJECT_NAME}Config.cmake
)

include(CMakePackageConfigHelpers)

write_basic_package_version_file("${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)

install(FILES "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake"
    DESTINATION "${CMAKECONFIG_INSTALL_DIR}"
)

configure_file(${PROJECT_NAME}.pc.in "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.pc" @ONLY)
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.pc"
    DESTINATION "${LIB_INSTALL_DIR}/pkgconfig"
)
