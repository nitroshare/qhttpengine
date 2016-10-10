configure_file(qhttpengine_global.h.in "${CMAKE_CURRENT_BINARY_DIR}/qhttpengine_global.h")

file(GLOB HEADERS "${CMAKE_SOURCE_DIR}/include/${PROJECT_NAME}/*")
set(HEADERS "${HEADERS}" "${CMAKE_CURRENT_BINARY_DIR}/qhttpengine_global.h")

set(SRC
    qfilesystemhandler.cpp
    qhttphandler.cpp
    qhttpparser.cpp
    qhttprange.cpp
    qhttpserver.cpp
    qhttpsocket.cpp
    qibytearray.cpp
    qiodevicecopier.cpp
    qlocalfile.cpp
    qobjecthandler.cpp
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

set_target_properties(QHttpEngine PROPERTIES
    CXX_STANDARD 11
    CXX_STANDARD_REQUIRED ON
    DEFINE_SYMBOL QT_NO_SIGNALS_SLOTS_KEYWORDS
    DEFINE_SYMBOL QHTTPENGINE_LIBRARY
    PUBLIC_HEADER "${HEADERS}"
    VERSION ${PROJECT_VERSION}
    SOVERSION ${PROJECT_VERSION_MAJOR}
)

target_include_directories(QHttpEngine PUBLIC
    "$<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}>/include"
    "$<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>"
    "$<INSTALL_INTERFACE:${INCLUDE_INSTALL_DIR}>"
)

target_link_libraries(QHttpEngine Qt5::Network)

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
