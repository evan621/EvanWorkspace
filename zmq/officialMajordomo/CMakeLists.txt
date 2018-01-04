project(testzmq)
cmake_minimum_required(VERSION 3.0)

include_directories ("${PROJECT_SOURCE_DIR}/include")

add_executable (mdclient mdclient.cpp)
target_link_libraries(mdclient ${PROJECT_SOURCE_DIR}/lib/libzmq.so)

add_executable (mdworker mdworker.cpp)
target_link_libraries(mdworker ${PROJECT_SOURCE_DIR}/lib/libzmq.so)

add_executable (mdbroker mdbroker.cpp)
target_link_libraries(mdbroker ${PROJECT_SOURCE_DIR}/lib/libzmq.so)

add_executable (rrclient requestreplycli.cpp)
target_link_libraries(rrclient ${PROJECT_SOURCE_DIR}/lib/libzmq.so)

add_executable (rrserver requestreplyser.cpp)
target_link_libraries(rrserver ${PROJECT_SOURCE_DIR}/lib/libzmq.so)
