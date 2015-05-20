set(SRC
    qiodevicecounter.cpp
    qsocketpair.cpp
)

add_library(common STATIC ${SRC})
qt5_use_modules(common Network)
