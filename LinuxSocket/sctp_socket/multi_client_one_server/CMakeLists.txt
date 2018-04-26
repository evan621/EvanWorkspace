project(sctp)

add_executable(client sctpclnt.c sctpnotification.c)
target_link_libraries (client sctp)

add_executable(server sctpsrvr.c sctpnotification.c)
target_link_libraries (server sctp)
