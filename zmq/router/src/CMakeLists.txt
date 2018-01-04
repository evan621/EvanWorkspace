find_package(Threads REQUIRED)

add_executable (zmqRouter zmqRouter.cpp)
target_link_libraries(zmqRouter ${PROJECT_SOURCE_DIR}/lib/libzmq.so.5.1.2)
target_link_libraries(zmqRouter Threads::Threads)
