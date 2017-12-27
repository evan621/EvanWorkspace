project(po)
cmake_minimum_required(VERSION 3.5)
set(CMAKE_CXX_STANDARD 14)

find_package(Boost COMPONENTS system ... REQUIRED)

if(Boost_FOUND)
	include_directories(${Boost_INCLUDE_DIRS})
		
	add_executable(poo po.cpp)
	target_link_libraries (poo ${Boost_LIBRARIES})

endif()
