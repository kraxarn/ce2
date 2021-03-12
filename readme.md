# CrowEngine v3
Version 3 of my custom game engine built on top of SDL2, using C++11. Currently, very early in
development and not quite ready for use in production just yet.

Check out the [v2](https://github.com/kraxarn/crowengine/tree/v2) branch for the old version based
on SFML/Allegro, no longer maintained or actively developed.

The design of the engine is very borrowed from SFML, although, it has nothing else to do with 
the library. Here are some differences:

|               | SFML                  | CrowEngine                           |
| ------------- | --------------------- | ------------------------------------ |
| Networking    | Full HTTP/FTP support | TCP/UDP sockets only                 |
| Shaders       | GLSL                  | Not supported                        |
| Assets        | From file or custom   | Built-in asset manager               |
| GUI           | No                    | Built-in GUI library and tile editor |
| Documentation | Well documented       | Auto-generated only                  |
| Platforms     | Desktop               | Desktop, mobile, web, etc.           |

So why use something like this over SFML? Portability of SDL I guess.

## External dependencies
External dependencies required to build/run the engine:
* [SDL2](https://github.com/libsdl-org/SDL)
* [SDL2_ttf](https://github.com/libsdl-org/SDL_ttf)