#pragma once
//Vulkan Header
#include <vulkan/vulkan.h>
//SDL2
#include <SDL2/SDL.h>
#include <SDL2/SDL_vulkan.h>

#include <iostream>
#include <sstream>
#include <vector>
#include <array>

#define dprint(V) { std::cout << V ; }

class Vulkan
{
private:
	SDL_Window *window;
	uint32_t screen_width;
	uint32_t screen_height;

	VkDevice device_context;
	VkQueue queue;
	uint32_t family_i;

	VkSwapchainKHR swapchain = VK_NULL_HANDLE;
	VkPresentModeKHR present_mode = VK_PRESENT_MODE_IMMEDIATE_KHR;
	std::vector<VkImage> swapchain_buffers;
	std::vector<VkImageView> swapchain_buffer_view;
	uint32_t active_swapchain_id = UINT32_MAX;
	VkSemaphore present = VK_NULL_HANDLE;
	VkSemaphore render = VK_NULL_HANDLE;
	std::vector<VkFence> swapchain_available;
	VkImage depth_stencil_buffer = VK_NULL_HANDLE;
	VkDeviceMemory depth_stencil_buffer_memory = VK_NULL_HANDLE;
	VkImageView depth_stencil_buffer_view = VK_NULL_HANDLE;
	VkRenderPass render_pass = VK_NULL_HANDLE;
	std::vector<VkFramebuffer> framebuffers;
	VkViewport viewport = {};
	VkRect2D scissor = {};
	VkPhysicalDeviceFeatures gpu_features;
	VkPipelineRasterizationStateCreateInfo rasterizer = {VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO};
	VkPipelineMultisampleStateCreateInfo multisampling = { VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO };
	VkPipelineLayout pipeline_layout;
	VkPipeline pipeline;

	VkResult result;
	VkInstance instance;
	VkSurfaceKHR window_surface;
	VkSwapchainKHR old_swapchain = nullptr;
	uint32_t swapchain_buffer_count = 2;
	VkSurfaceCapabilitiesKHR surface_caps {};
	VkFormat depth_buffer_format = VK_FORMAT_UNDEFINED;
	bool stencil_support = false;
	bool present_mode_set = true;
	VkSurfaceFormatKHR surface_format {};
	VkPhysicalDevice device;
	VkPhysicalDeviceProperties device_properties {};
	VkPhysicalDeviceMemoryProperties device_memory_info {};
	std::vector<const char *> device_extensions;
	std::vector<const char *> instance_layers;
	std::vector<const char *> instance_extensions;
	VkDebugReportCallbackCreateInfoEXT debug_create_info = {};

public:
	Vulkan (SDL_Window *window, uint32_t screen_width, uint32_t screen_height);
	~Vulkan ();

	//Rendering Functions (you can do all this stuff manually in your game loop using the public variables available)
	void AcquireNextSwapchain ();
	void BeginRenderPresent (std::vector<VkCommandBuffer> command_buffers);
	//~Rendering Functions

private:
	void GetSDLWindowInfo ();
	void InitInstance ();
	void DestroyInstance ();

	void ShowAvailableExtensions ();

	void SelectDevice ();
	void CreateDeviceContext ();
	void DestroyDeviceContext ();

	void CreateSurface (SDL_Window *window);
	void DestroySurface ();

	void CreateSwapchain ();
	void DestroySwapchain ();

	void CreateSwapchainImages ();
	void DestroySwapchainImages ();

	void CreateDepthStencilImage ();
	void DestroyDepthStencilImage ();

	void CreateRenderPass ();
	void DestroyRenderPass ();

	void CreateFramebuffers ();
	void DestroyFramebuffers ();

	void StartSynchronizations ();
	void EndSynchronizations ();

	static const char* get_device_type_str (VkPhysicalDeviceType type);

#ifdef VK_DEBUG
	VkDebugReportCallbackEXT debug_report;
	void SetupDebug ();
	void InitDebug ();
	void DestroyDebug ();
#endif
};
