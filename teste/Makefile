BOX2D_LIB=box2d/build/bin/libbox2d.a
BOX2D_INCLUDE=box2d/include

all:
	g++ -o teste teste.cpp -I$(BOX2D_INCLUDE) $(BOX2D_LIB) -std=c++11 `pkg-config --cflags --libs sdl2` -ggdb
