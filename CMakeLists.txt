cmake_minimum_required(VERSION 3.14)
project(CrowEngine)

set(CMAKE_CXX_STANDARD 14)

include_directories(include)
link_directories(lib)

add_compile_definitions(SFML_STATIC)
add_compile_definitions(ALLEGRO_STATICLINK)

set(LIBS "-lsfml-graphics-s -lsfml-window-s -lsfml-system-s -lallegro_acodec-static -lallegro_audio-static -lallegro_physfs-static -lallegro-static -lfreetype -lGL -lGLU -ldumb -lm -lpulse-simple -lpulse -lasound -lphysfs -lpthread -lbox2d -lfmt")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${LIBS} -Werror")

add_executable(CrowEngine
        src/AnimatedSprite.cpp
        src/Animation.cpp
        src/CrowEngine.cpp
        src/main.cpp)