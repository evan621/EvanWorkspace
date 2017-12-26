project(sctp)

set(CMAKE_CXX_STANDARD 14)

add_executable(client SctpClientMain.cpp SctpClientEndpoint.cpp SctpNotification.cpp)
target_link_libraries (client sctp)

#add_executable(clientc sctpclnt.c)
#target_link_libraries (clientc sctp)

add_executable(server SctpServerMain.cpp SctpServerEndpoint.cpp SctpNotification.cpp)
target_link_libraries (server sctp)
