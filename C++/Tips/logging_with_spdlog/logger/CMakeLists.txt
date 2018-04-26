project(spdlog_example)
cmake_minimum_required(VERSION 3.4)

set(CMAKE_CXX_STANDARD 14)

add_library(spdlog INTERFACE)


target_include_directories(
	spdlog
	INTERFACE
	"$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/../spdlog/include>"
)

find_package(Threads)

add_executable(spdexam Logger.cpp Main.cpp)
target_link_libraries(spdexam spdlog ${CMAKE_THREAD_LIBS_INIT})


add_executable(loggeragent LoggerAgent.cpp)
