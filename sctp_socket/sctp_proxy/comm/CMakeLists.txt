
include_directories(../include ../spdlog/include)

set(SOURCES
	SctpServerEndpoint.cpp
	SctpClientEndpoint.cpp
	DomainSocketServerEndpoint.cpp
	DomainSocketClientEndpoint.cpp
	SctpNotification.cpp 
	SctpSocket.cpp 
	IoMultiplex.cpp
	DomainSocket.cpp)

add_library(common ${SOURCES})
