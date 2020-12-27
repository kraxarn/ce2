cmake_minimum_required(VERSION 3.14)
project(CrowEngine)

set(CMAKE_CXX_STANDARD 14)

include_directories(include)
link_directories(lib)

add_compile_definitions(SFML_STATIC)
add_compile_definitions(ALLEGRO_STATICLINK)

# Windows libraries
#[[
    winmm.lib
    ws2_32.lib
    opengl32.lib
    gdi32.lib
    freetype.lib
    jpeg.lib
    sfml-system-s.lib
    sfml-network-s.lib
    sfml-window-s.lib
    sfml-graphics-s.lib
    dxguid.lib
    ole32.lib
    dinput8.lib
    dsound.lib
    gdiplus.lib
    psapi.lib
    shlwapi.lib
    dumb.lib
    allegro-static.lib
    allegro_main-static.lib
    allegro_audio-static.lib
    allegro_acodec-static.lib
    allegro_dialog-static.lib
    allegro_physfs-static.lib
    Box2D.lib
    physfs-static.lib
    ComCtl32.Lib
    ComDlg32.Lib
]]

#set(SFML_DEPS "-lfreetype -lGL -lGLU")

#set(SFML_LIBS "-lsfml-graphics -lsfml-window -lsfml-system")
#set(SFML_LIBS "-lsfml-system -lsfml-window -lsfml-graphics -lsfml-audio -lsfml-network")

#set(ALLEGRO_DEPS "-ldumb -lm -lpulse-simple -lpulse -lasound -lphysfs -lpthread")

#set(ALLEGRO_LIBS, "-lallegro-static -lallegro_main-static -lallegro_audio-static -lallegro_acodec-static -lallegro_physfs-static")
#set(ALLEGRO_LIBS "-lallegro_acodec-static -lallegro_audio-static -lallegro_physfs-static -lallegro-static")

set(LIBS "-lsfml-graphics-s -lsfml-window-s -lsfml-system-s -lallegro_acodec-static -lallegro_audio-static -lallegro_physfs-static -lallegro-static -lfreetype -lGL -lGLU -ldumb -lm -lpulse-simple -lpulse -lasound -lphysfs -lpthread -lBox2D")

#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -lBox2D ${SFML_DEPS} ${SFML_LIBS} ${ALLEGRO_DEPS} ${ALLEGRO_LIBS}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${LIBS} -Werror")

add_executable(CrowEngine
        src/AnimatedSprite.cpp
        src/Animation.cpp
        src/CrowEngine.cpp
        src/main.cpp
        src/physfs.cpp
        src/fmt/format.cc
        src/fmt/ostream.cc
        src/fmt/posix.cc
        src/fmt/printf.cc)