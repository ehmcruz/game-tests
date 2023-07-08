# test vulkan
# vkcube

all:
	glslc shaders/triangle.vert -o shaders/triangle.vert.spv
	glslc shaders/triangle.frag -o shaders/triangle.frag.spv
	g++ -o hello main.cpp Renderer.cpp pipeline.cpp -lvulkan `sdl2-config --cflags --libs` -ggdb