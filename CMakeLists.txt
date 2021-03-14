cmake_minimum_required(VERSION 3.5)
project(crowengine LANGUAGES CXX)

# C++11
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Options
option(CE_SYSTEM_LIBS "Use system libraries" OFF)

# Project files
file(GLOB MAIN_SRC "src/*.cpp")
file(GLOB MAIN_INC "include/ce/*.hpp")
file(GLOB SUB_INC "include/ce/*/*.hpp")
add_library(crowengine STATIC ${MAIN_SRC} ${MAIN_INC} ${SUB_INC})

# SDL libraries
if (CE_SYSTEM_LIBS)
	# CMake support is broken on some systems, so use pkg-config instead
	find_package(PkgConfig REQUIRED)
	pkg_check_modules(SDL2 REQUIRED sdl2)
	pkg_check_modules(SDL2ttf REQUIRED SDL2_ttf)
	# Headers
	include_directories(${SDL2_INCLUDE_DIRS})
	include_directories(${SDL2ttf_INCLUDE_DIRS})
	# Libraries
	link_directories(${SDL2_LIBRARY_DIRS})
	link_directories(${SDL2ttf_LIBRARY_DIRS})
else ()
	# TODO: Customize build for features of engine
	# Project directories
	add_subdirectory(thirdparty/SDL)
	add_subdirectory(thirdparty/SDL_ttf)
	# Link
	target_link_libraries(crowengine PRIVATE SDL2 SDL_ttf)
endif ()
