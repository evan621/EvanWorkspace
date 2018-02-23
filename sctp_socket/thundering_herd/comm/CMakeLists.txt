
include_directories(../include)

set(SOURCES
	SctpServerEndpoint.cpp
	SctpClientEndpoint.cpp
	DomainSocketServerEndpoint.cpp
	DomainSocketClientEndpoint.cpp
	SctpNotification.cpp 
	SctpSocketOperation.cpp 
	IoMultiplex.cpp)

add_library(common ${SOURCES})
