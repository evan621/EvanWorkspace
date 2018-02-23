project(cpif)
cmake_minimum_required(VERSION 3.5)

set(CMAKE_CXX_STANDARD 14)

set(EXECUTABLE_NAME server)
set(SOURCES
    main.cpp
	worker.cpp
	master.cpp)

include_directories(${PROJECT_SOURCE_DIR}/include)

add_subdirectory(comm)

add_executable(${EXECUTABLE_NAME} ${SOURCES})
target_link_libraries(${EXECUTABLE_NAME} common sctp)

add_executable(loadAndTest	test.cpp)
target_link_libraries(loadAndTest common sctp)


