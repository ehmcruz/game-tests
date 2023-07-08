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
#include <optional>

#include <cstdint>

#define dprint(V) { std::cout << V ; }

class Vulkan
{
	friend class VulkanShader;
	friend class VulkanPipeline;

private:
	struct QueueFamilyIndices {
		std::optional<uint32_t> graphics_family;
		std::optional<uint32_t> present_family;
	};

public:
	inline uint32_t get_graphics_family ()
	{
		return *(this->family_indices.graphics_family);
	}

public:
	SDL_Window *window;
	uint32_t screen_width;
	uint32_t screen_height;

	VkInstance instance;
	VkPhysicalDevice device;
	VkPhysicalDeviceProperties device_properties {};
	VkPhysicalDeviceMemoryProperties device_memory_info {};
	VkPhysicalDeviceFeatures device_features; // old gpu_features
	VkDevice device_context;

	VkQueue graphics_queue;
	VkQueue present_queue;
	QueueFamilyIndices family_indices; //uint32_t family_i;

	VkSurfaceKHR surface; // old window_surface
	VkSurfaceCapabilitiesKHR surface_caps {};
	VkSurfaceFormatKHR surface_format {};
	VkPresentModeKHR present_mode;
	VkExtent2D extent; // render dimensions

	uint32_t swapchain_buffer_count;
	VkSwapchainKHR swapchain = VK_NULL_HANDLE;
	std::vector<VkImage> swapchain_buffers;
	std::vector<VkImageView> swapchain_buffer_view;

	uint32_t active_swapchain_id = std::numeric_limits<uint32_t>::max();

	VkRenderPass render_pass = VK_NULL_HANDLE;

	std::vector<VkFramebuffer> framebuffers;

	VkSemaphore sem_present; // image available semaphore
	VkSemaphore sem_render;
	VkFence in_flight_fence;

	std::vector<const char *> device_extensions;
	std::vector<const char *> instance_layers;
	std::vector<const char *> instance_extensions;

#if 0
	std::vector<VkFence> swapchain_available;
	VkImage depth_stencil_buffer = VK_NULL_HANDLE;
	VkDeviceMemory depth_stencil_buffer_memory = VK_NULL_HANDLE;
	VkImageView depth_stencil_buffer_view = VK_NULL_HANDLE;

	//VkResult result;
	VkSwapchainKHR old_swapchain = nullptr;
	VkFormat depth_buffer_format = VK_FORMAT_UNDEFINED;
	bool stencil_support = false;

	VkDebugReportCallbackCreateInfoEXT debug_create_info = {};
#endif

public:
	Vulkan (SDL_Window *window, uint32_t screen_width, uint32_t screen_height);
	~Vulkan ();

	//Rendering Functions (you can do all this stuff manually in your game loop using the public variables available)
	void AcquireNextSwapchain ();
	void BeginRenderPresent (VkCommandBuffer& command_buffer);
	//~Rendering Functions

private:
	void GetSDLWindowInfo ();
	void InitInstance ();
	void DestroyInstance ();

	void ShowAvailableInstanceExtensions ();
	bool CheckDeviceExtensionSupport (VkPhysicalDevice device);

	void SelectDevice ();
	QueueFamilyIndices findQueueFamilies (VkPhysicalDevice device);
	void CreateDeviceContext ();
	void DestroyDeviceContext ();

	void QuerySurface ();
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
	static std::string get_family_queues_str (VkQueueFlags flags);

#ifdef VK_DEBUG
	VkDebugReportCallbackEXT debug_report;
	void SetupDebug ();
	void InitDebug ();
	void DestroyDebug ();
#endif
};
