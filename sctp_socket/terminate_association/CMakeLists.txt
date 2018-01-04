project(sctp)
cmake_minimum_required(VERSION 3.5)

set(CMAKE_CXX_STANDARD 14)

find_package (Threads)


#add_executable(client SctpClientMain.cpp SctpClientEndpoint.cpp SctpNotification.cpp SctpSocketOperation.cpp)
#target_link_libraries (client sctp ${CMAKE_THREAD_LIBS_INIT})

add_executable(server SctpServerMain.cpp SctpServerEndpoint.cpp SctpNotification.cpp SctpSocketOperation.cpp IoMultiplex.cpp)
target_link_libraries (server sctp ${CMAKE_THREAD_LIBS_INIT})
