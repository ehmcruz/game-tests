all:
	g++ -o sdl-play-sound sdl-play-sound.cpp `pkg-config --cflags --libs sdl2 SDL2_mixer`

mingw:
	g++ -o sdl-play-sound.exe sdl-play-sound.cpp -I d:\mingw\SDL2-2.0.9\x86_64-w64-mingw32\include\SDL2\ -LD:\mingw\SDL2-2.0.9\x86_64-w64-mingw32\lib\ -lSDL2 -lSDL2main -lmingw32 -w
