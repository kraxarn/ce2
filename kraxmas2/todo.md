* Try to update dependencies
* Recompile SFML (and Allegro) as Release
* Make each world a seperate level (Like acts in Sonic)
* Add some sort of warning 'Debug Mode' text
* Implement logger better (Or just use the console)
* Try to compile SFML without some deps or compile source with engine
* Move all functions to CrowEngine.cpp (but leave the headers in .hpp)
* TGA (or even GIF) compresses better than PNG in zip files
* Probably fix level memory leak by not using 'new' for scenes
* 2 sound mixers, one for music, one for sfx?
* BUG: Always use sizeof() since sizeof is VS exclusive
*
* MAYBE: If completed game and resuming, just show end screen
* MAYBE: Add option to change used controller
* MAYBE: Fix always ducking with mega drive controller (Can this be fixed?)
* MAYBE: Fix transition not transitioning properly
* MAYBE: Fix transition not working correctly after vieweing EndScreen more than once

## Cleaned up scenes

* SceneEndGame - Good
* SceneHUD - Clean
* SceneLevel - Clean
* SceneMenu - Clean
* SceneSubMenu - Cleaned
*
* Clean: Not cleaned yet
* Cleaned: Unused vars cleaned
* Good: Unused cars, unused code and todos cleaned

## Taco size

* taco is ~57 kb smaller if using 7z
* engine is ~11 kb larger (~6 kb compressed)

## Build types

* Allegro - Release - (Release is ~65 kb larger, ~10 kb compressed)
* SFML - MinSizeRel - (Release is ~66 kb larger, ~22 kb compressed)
* Deps - Release