set(SRC
    qsimplehttpclient.cpp
    qsocketpair.cpp
)

add_library(common STATIC ${SRC})
target_link_libraries(common QHttpEngine)

if (Qt4_FOUND)
    target_link_libraries(common Qt4::QtNetwork)  
else()
    qt5_use_modules(common Network)
endif()
