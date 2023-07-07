# test vulkan
# vkcube

VULKAN_PATH=/VulkanSDK/1.3.250.1
VULKAN_INCLUDE=$(VULKAN_PATH)/Include
VULKAN_LIB=$(VULKAN_PATH)/Include

all:
	g++ -o hello main.cpp Renderer.cpp -lvulkan-1 `sdl2-config --cflags --libs`

windows:
	g++ -o hello main.cpp Renderer.cpp `sdl2-config --cflags --libs` -I$(VULKAN_INCLUDE)