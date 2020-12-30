cmake_minimum_required(VERSION 3.14)
project(CrowEngine)

set(CMAKE_CXX_STANDARD 17)

# Allow using system libraries
set(USE_SYSTEM_LIBS OFF)
if (USE_SYSTEM_LIBS)
	message(STATUS "Using system libraries")
elseif (NOT USE_SYSTEM_LIBS)
	message(STATUS "Using bundled libraries")
endif ()

# Engine headers
include_directories(include)
include_directories(src)

# Bundled libraries
link_directories(lib/lib)
include_directories(lib/include)

if (USE_SYSTEM_LIBS)
	# pkg-config for some dependencies
	find_package(PkgConfig REQUIRED)
	# SFML
	find_package(SFML 2 COMPONENTS system window graphics REQUIRED)
	# Allegro (pkg-config)
	pkg_check_modules(allegro REQUIRED allegro-5 allegro_physfs-5 allegro_audio-5 allegro_acodec-5)
	# Box2D
	#find_package(box2d REQUIRED)
	# fmt
	find_package(fmt REQUIRED)
	# PhysFS
	pkg_check_modules(PhysicsFS REQUIRED physfs)
endif ()

# Non-system libraries are static
if (NOT USE_SYSTEM_LIBS)
	add_compile_definitions(SFML_STATIC)
	add_compile_definitions(ALLEGRO_STATICLINK)
endif ()

# Source files
add_executable(CrowEngine
		src/AnimatedSprite.cpp
		src/Animation.cpp
		src/CrowEngine.cpp
		src/main.cpp)

# Custom legacy Box2D
target_link_libraries(CrowEngine PRIVATE Box2D)

# Debug mode logs OpenGL information
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
	find_package(OpenGL REQUIRED)
	target_link_libraries(CrowEngine PRIVATE GL)
	target_compile_definitions(CrowEngine PRIVATE CE_DEBUG)
endif ()

# Libraries
if (USE_SYSTEM_LIBS)
	target_link_libraries(CrowEngine PRIVATE
			sfml-system
			sfml-window
			sfml-graphics
			Box2D
			fmt
			${allegro_LIBRARIES}
			${PhysicsFS_LIBRARIES})
else ()
	target_link_libraries(CrowEngine PRIVATE
			# sfml static dependencies
			pthread
			X11
			Xrandr
			GL
			GLU
			udev
			freetype
			# allegro_audio static dependencies
			pulse-simple
			pulse
			asound
			# allegro_acodec static dependencies
			dumb-static
			m
			Xcursor
			Xi
			Xpm
			# sfml
			sfml-system-s
			sfml-window-s
			sfml-graphics-s
			# Box2D
			Box2D
			# fmt
			fmt-static
			# allegro
			allegro-static
			allegro_main-static
			allegro_acodec-static
			allegro_audio-static
			allegro_physfs-static
			# physfs
			physfs-static)
endif ()