cmake_minimum_required(VERSION 2.8)

project(read_lis3dh C)



add_compile_options(
	-Wall
	-Wno-deprecated-declarations
	-Wno-enum-compare
	-Wno-unused-local-typedefs
	-Wno-unused-parameter
	)

add_definitions(-DREAD_LIS3DH_ROOT_DIR=\"${CMAKE_CURRENT_SOURCE_DIR}\")


add_subdirectory(src)
	
