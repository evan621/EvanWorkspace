project(tutorial-0)
cmake_minimum_required(VERSION 3.5)
set(CMAKE_CXX_STANDARD 14)

set(BOOST_ROOT /usr/local/install/boost_1_61_0)

find_package(Boost COMPONENTS regex system REQUIRED)

if(Boost_FOUND)
	include_directories(${Boost_INCLUDE_DIRS})

	add_executable(foo foo.cpp)
	target_link_libraries (foo ${Boost_LIBRARIES})
endif()
