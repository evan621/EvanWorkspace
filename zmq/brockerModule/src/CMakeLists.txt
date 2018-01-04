add_executable (mdclient mdclient.cpp)
target_link_libraries(mdclient ${PROJECT_SOURCE_DIR}/lib/libzmq.so)

#add_executable (mdworker mdworker.cpp)
#target_link_libraries(mdworker ${PROJECT_SOURCE_DIR}/lib/libzmq.so)

add_executable (mdbroker mdbroker.cpp)
target_link_libraries(mdbroker ${PROJECT_SOURCE_DIR}/lib/libzmq.so)
