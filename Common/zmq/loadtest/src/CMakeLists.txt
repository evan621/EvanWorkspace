add_executable (hwclient hwclient.cpp)
target_link_libraries(hwclient ${PROJECT_SOURCE_DIR}/lib/libzmq.so.5.1.2)

#add_executable (mdworker mdworker.cpp)
#target_link_libraries(mdworker ${PROJECT_SOURCE_DIR}/lib/libzmq.so)

add_executable (hwserver hwserver.cpp)
target_link_libraries(hwserver ${PROJECT_SOURCE_DIR}/lib/libzmq.so.5.1.2)
