cmake_minimum_required(VERSION 3.14)
project(CrowEngine)

set(CMAKE_CXX_STANDARD 17)

include_directories(include)

# pkg-config for some dependencies
find_package(PkgConfig REQUIRED)

# SFML
find_package(SFML 2 COMPONENTS system window graphics REQUIRED)
# Allegro (pkg-config)
pkg_check_modules(allegro REQUIRED allegro-5 allegro_physfs-5 allegro_audio-5 allegro_acodec-5)
# Box2D
find_package(box2d REQUIRED)
# fmt
find_package(fmt REQUIRED)
# PhysFS
pkg_check_modules(PhysicsFS REQUIRED physfs)

# Source files
add_executable(CrowEngine
		src/AnimatedSprite.cpp
		src/Animation.cpp
		src/CrowEngine.cpp
		src/main.cpp)

# Libraries
target_link_libraries(CrowEngine
		sfml-system
		sfml-window
		sfml-graphics
		box2d
		fmt
		${allegro_LIBRARIES}
		${PhysicsFS_LIBRARIES})
