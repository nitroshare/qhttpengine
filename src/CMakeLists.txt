configure_file(qhttpengine.h.in "${CMAKE_CURRENT_BINARY_DIR}/qhttpengine.h")

file(GLOB HEADERS "${PROJECT_NAME}/*")
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
)

if(WIN32)
    set(OUTPUT_NAME ${PROJECT_NAME}${PROJECT_VERSION_MAJOR})
    configure_file(resource.rc.in "${CMAKE_CURRENT_BINARY_DIR}/resource.rc")
    set(SRC ${SRC} "${CMAKE_CURRENT_BINARY_DIR}/resource.rc")
else()
    set(OUTPUT_NAME ${PROJECT_NAME})
endif()

add_library(qhttpengine SHARED ${HEADERS} ${SRC})
qt5_use_modules(qhttpengine Network)

target_include_directories(qhttpengine PUBLIC
    "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>"
    "$<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>"
    "$<INSTALL_INTERFACE:${INCLUDE_INSTALL_DIR}>"
)

set_target_properties(qhttpengine PROPERTIES
    DEFINE_SYMBOL QT_NO_SIGNALS_SLOTS_KEYWORDS
    DEFINE_SYMBOL QHTTPENGINE_LIBRARY
    PUBLIC_HEADER "${HEADERS}"
    OUTPUT_NAME ${OUTPUT_NAME}
    VERSION ${PROJECT_VERSION}
    SOVERSION ${PROJECT_VERSION_MAJOR}
    RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}"
    LIBRARY_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}"
)

install(TARGETS qhttpengine EXPORT qhttpengine-export
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION "${LIB_INSTALL_DIR}"
    ARCHIVE DESTINATION "${LIB_INSTALL_DIR}"
    PUBLIC_HEADER DESTINATION "${INCLUDE_INSTALL_DIR}/${PROJECT_NAME}"
)

install(EXPORT qhttpengine-export DESTINATION "${CMAKECONFIG_INSTALL_DIR}"
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

if(NOT WIN32)
    configure_file(${PROJECT_NAME}.pc.in "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.pc" @ONLY)
    install(FILES "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.pc"
        DESTINATION "${LIB_INSTALL_DIR}/pkgconfig"
    )
endif()
