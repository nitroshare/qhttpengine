# Client
add_executable(authclient client.cpp)
target_link_libraries(authclient QHttpEngine)
target_compile_features(authclient PRIVATE cxx_lambdas)
install(TARGETS authclient
    RUNTIME DESTINATION "${EXAMPLE_DIR}"
)
