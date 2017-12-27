project(tutorial-0)
cmake_minimum_required(VERSION 3.5)
set(CMAKE_CXX_STANDARD 14)

find_package(Boost REQUIRED)

if(Boost_FOUND)
	include_directories(${Boost_INCLUDE_DIRS})
	
	add_executable(foo foo.cpp)
endif()
