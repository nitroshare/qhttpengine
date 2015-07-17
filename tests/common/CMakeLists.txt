set(SRC
    qsimplehttpclient.cpp
    qsocketpair.cpp
)

add_library(common STATIC ${SRC})
target_link_libraries(common QHttpEngine)

qt5_use_modules(common Network)
