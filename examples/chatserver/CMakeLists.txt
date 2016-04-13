set(SRC
    apihandler.cpp
    main.cpp
)

if (Qt4_FOUND)
    QT4_ADD_RESOURCES(QRC resources.qrc)
else()
    qt5_add_resources(QRC resources.qrc)
endif()

add_executable(chatserver ${SRC} ${QRC})
target_link_libraries(chatserver QHttpEngine)

install(TARGETS chatserver
    RUNTIME DESTINATION "${EXAMPLE_DIR}"
)
