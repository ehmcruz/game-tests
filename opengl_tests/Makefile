all:
	g++ -o game game.cpp -std=c++11 `pkg-config --cflags --libs sdl2`

mingw:
	g++ -o game game.cpp -std=c++11 `pkg-config --cflags --libs sdl2` -static-libgcc -static-libstdc++

mingw2:
	g++ -o game game.cpp -std=c++11 `sdl2-config --cflags --libs` -static-libgcc -static-libstdc++

mingw3:
	g++ -o game.exe game.cpp -I d:\mingw\SDL2-2.0.9\x86_64-w64-mingw32\include\SDL2\ -LD:\mingw\SDL2-2.0.9\x86_64-w64-mingw32\lib\ -lSDL2 -lSDL2main -lmingw32 -w

mingw4:
	echo `sdl2-config --cflags`
	echo `sdl2-config --libs`
	g++ -o game.exe game.cpp `sdl2-config --cflags` `sdl2-config --libs` -mconsole -lglew32 -lopengl32

triangle: triangle.cpp
	echo `sdl2-config --cflags`
	echo `sdl2-config --libs`
	g++ -o triangle.exe triangle.cpp `sdl2-config --cflags` `sdl2-config --libs` -mconsole -lglew32 -lopengl32

texture: texture.cpp
	echo `sdl2-config --cflags`
	echo `sdl2-config --libs`
	g++ -o texture.exe texture.cpp `sdl2-config --cflags` `sdl2-config --libs` -lSDL2_image -mconsole -lglew32 -lopengl32 -ggdb

texture-array: texture-array.cpp
	echo `sdl2-config --cflags`
	echo `sdl2-config --libs`
	g++ -o texture-array.exe texture-array.cpp `sdl2-config --cflags` `sdl2-config --libs` -lSDL2_image -mconsole -lglew32 -lopengl32 -ggdb

circle-vs-2-linux: circle-vs-2.cpp
	echo `sdl2-config --cflags`
	echo `sdl2-config --libs`
	g++ -o circle-vs-2 circle-vs-2.cpp `sdl2-config --cflags` `sdl2-config --libs` -lGL -lGLEW -lm -ggdb

circle-vs: circle-vs.cpp
	echo `sdl2-config --cflags`
	echo `sdl2-config --libs`
	g++ -o circle-vs.exe circle-vs.cpp `sdl2-config --cflags` `sdl2-config --libs` -lSDL2_image -mconsole -lglew32 -lopengl32 -lm -ggdb

circle-fs: circle-fs.cpp
	echo `sdl2-config --cflags`
	echo `sdl2-config --libs`
	g++ -o circle-fs.exe circle-fs.cpp `sdl2-config --cflags` `sdl2-config --libs` -lSDL2_image -mconsole -lglew32 -lopengl32 -lm -ggdb

mingw5:
	g++ -o game game.cpp `sdl2-config --cflags --libs` -lglew32

mingw6:
	g++ -o game game.cpp `sdl2-config --cflags --libs` -lglew32

# gcc -o cube-3 cube-3.c -L/mingw32/lib -lmingw32 -lSDL2main -lSDL2 -lglew32 -opengl32 -lglu32 -mwindows -I/mingw32/include/    SDL2 -Dmain=SDL_main
