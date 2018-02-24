project(cpif)
cmake_minimum_required(VERSION 3.5)

set(CMAKE_CXX_STANDARD 14)

# 往工程中添加spdlog库，这个库是一个interface类型的库
add_library(spdlog INTERFACE)

set(EXECUTABLE_NAME server)
set(SOURCES
    main.cpp
	worker.cpp
	master.cpp)

	
# 包含这个interface库的工程路径
target_include_directories(
    spdlog
    INTERFACE
    "$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/spdlog/include>"
)

include_directories(${PROJECT_SOURCE_DIR}/include)

# spdlog会用到pthread库
find_package(Threads)

add_subdirectory(comm)

add_executable(${EXECUTABLE_NAME} ${SOURCES})
target_link_libraries(${EXECUTABLE_NAME} common sctp  spdlog ${CMAKE_THREAD_LIBS_INIT})


add_executable(loadAndTest loadAndTest.cpp tester.cpp)
target_link_libraries(loadAndTest common sctp spdlog ${CMAKE_THREAD_LIBS_INIT})