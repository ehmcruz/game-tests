#include <array>
#include <set>
#include <algorithm>
#include <string_view>
#include <limits>

#include "Renderer.h"

const char* Vulkan::get_device_type_str (VkPhysicalDeviceType type)
{
	static const char *device_type[] = {
		"VK_PHYSICAL_DEVICE_TYPE_OTHER",
		"VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU",
		"VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU",
		"VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU",
		"VK_PHYSICAL_DEVICE_TYPE_CPU",
		"ERROR TYPE"
	};

	return ((type <= VK_PHYSICAL_DEVICE_TYPE_CPU) ? device_type[type] : device_type[5] );
}

std::string Vulkan::get_family_queues_str (VkQueueFlags flags)
{
	std::string str;

	if (flags & VK_QUEUE_GRAPHICS_BIT)
		str += "graphics,";
	if (flags & VK_QUEUE_COMPUTE_BIT)
		str += "compute,";
	if (flags & VK_QUEUE_TRANSFER_BIT)
		str += "transfer,";

	return str;
}

Vulkan::Vulkan (SDL_Window *window, uint32_t screen_width, uint32_t screen_height)
{
	//instance_extensions;
	this->window = window;
	this->screen_width = screen_width;
	this->screen_height = screen_height;

	this->GetSDLWindowInfo();

	dprint( "InitInstance start" << std::endl );
	this->InitInstance();
	dprint( "InitInstance end" << std::endl );

	this->ShowAvailableInstanceExtensions();

	if (!SDL_Vulkan_CreateSurface(this->window, this->instance, &(this->surface)))
		throw std::runtime_error("failed to create SDL Vulkan surface!");

	dprint( "SelectDevice start" << std::endl );
	this->SelectDevice();
	dprint( "SelectDevice end" << std::endl );

	dprint( "CreateDeviceContext start" << std::endl );
	this->CreateDeviceContext();
	dprint( "CreateDeviceContext end" << std::endl );

	dprint( "QuerySurface start" << std::endl );
	this->QuerySurface();
	dprint( "QuerySurface end" << std::endl );

	dprint( "CreateSwapchain start" << std::endl );
	this->CreateSwapchain();
	dprint( "CreateSwapchain end" << std::endl );

	dprint( "CreateSwapchainImages start" << std::endl );
	this->CreateSwapchainImages();
	dprint( "CreateSwapchainImages end" << std::endl );

	dprint( "CreateRenderPass start" << std::endl );
	this->CreateRenderPass();
	dprint( "CreateRenderPass end" << std::endl );

	dprint( "CreateFramebuffers start" << std::endl );
	this->CreateFramebuffers();
	dprint( "CreateFramebuffers end" << std::endl );

	dprint( "StartSynchronizations start" << std::endl );
	this->StartSynchronizations();
	dprint( "StartSynchronizations end" << std::endl );

#if 0
	this->CreateDepthStencilImage();
#endif
}

Vulkan::~Vulkan()
{
#if 0
	vkQueueWaitIdle(this->queue);

	EndSynchronizations();

	DestroyFramebuffers();
	DestroyRenderPass();
	DestroyDepthStencilImage();
	DestroySwapchainImages();
	DestroySwapchain();
	DestroySurface();
	DestroyDeviceContext();
	DestroyInstance();
#endif
}

void Vulkan::DestroyInstance ()
{
#ifdef VK_DEBUG
	DestroyDebug();
#endif  //VK_DEBUG Destroy Debug First

	vkDestroyInstance(this->instance, nullptr);
	this->instance = nullptr;
}

void Vulkan::DestroyDeviceContext ()
{
	vkDestroyDevice(this->device_context, nullptr);
	this->device_context = nullptr;
}

void Vulkan::DestroySurface ()
{
	vkDestroySurfaceKHR(this->instance, this->surface, nullptr);
}

void Vulkan::DestroySwapchain ()
{
	vkDestroySwapchainKHR(this->device_context, this->swapchain, nullptr);
}

void Vulkan::DestroySwapchainImages ()
{
	for (auto view : this->swapchain_buffer_view) {
		vkDestroyImageView(this->device_context, view, nullptr);
	}
}

void Vulkan::DestroyRenderPass ()
{
	vkDestroyRenderPass(this->device_context, this->render_pass, nullptr);
}

void Vulkan::DestroyFramebuffers ()
{
	for (auto buffer : this->framebuffers) {
		vkDestroyFramebuffer(this->device_context, buffer, nullptr);
	}
}

void Vulkan::EndSynchronizations()
{
	/*for (auto fence: swapchain_available) {
		vkDestroyFence(device_context,fence,nullptr);
	}*/
	vkDestroyFence(this->device_context, this->in_flight_fence, nullptr);
	vkDestroySemaphore(this->device_context, this->sem_render, nullptr);
	vkDestroySemaphore(this->device_context, this->sem_present, nullptr);
}

void Vulkan::GetSDLWindowInfo ()
{
	uint32_t extension_count = 0;

	SDL_Vulkan_GetInstanceExtensions(this->window, &extension_count, nullptr);
	this->instance_extensions.resize(extension_count);

	if (!SDL_Vulkan_GetInstanceExtensions(this->window, &extension_count, this->instance_extensions.data())) {
		throw std::runtime_error( SDL_GetError() );
	}

	dprint( "required instance_extensions:" << std::endl )
	for (const char *p: this->instance_extensions) {
		dprint( '\t' << p << std::endl )
	}

	this->device_extensions.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
}

void Vulkan::InitInstance ()
{
#ifdef VK_DEBUG
	SetupDebug();
#endif // VK_DEBUG add Extensions

	VkApplicationInfo appInfo = {};
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.apiVersion = VK_MAKE_VERSION(1, 0, 14);
	appInfo.applicationVersion = VK_MAKE_VERSION(0, 1, 0);
	appInfo.pApplicationName = "Hello Vulkan";
	/*appInfo.pEngineName = "No Engine";
	appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.apiVersion = VK_API_VERSION_1_0;*/

	VkInstanceCreateInfo InstanceInfo {};
	InstanceInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	InstanceInfo.pApplicationInfo = &appInfo;
	InstanceInfo.enabledLayerCount = this->instance_layers.size();
	InstanceInfo.ppEnabledLayerNames = this->instance_layers.data();
	InstanceInfo.enabledExtensionCount = this->instance_extensions.size();
	InstanceInfo.ppEnabledExtensionNames = this->instance_extensions.data();
	
	VkResult result = vkCreateInstance(&InstanceInfo, nullptr, &(this->instance));
	if (result != VK_SUCCESS) {
		throw std::runtime_error("Create Instance Failed");
	}

#ifdef VK_DEBUG
	InitDebug();
#endif
}

void Vulkan::ShowAvailableInstanceExtensions ()
{
	uint32_t extensionCount = 0;
	vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
	std::vector<VkExtensionProperties> extensions(extensionCount);
	vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());

	dprint( "available extensions:" << std::endl )

	for (const auto& extension: extensions) {
		dprint( '\t' << extension.extensionName << std::endl )
	}
}

void Vulkan::SelectDevice ()
{
	uint32_t gpu_number = 0;
	vkEnumeratePhysicalDevices(this->instance, &gpu_number, nullptr);

	if (gpu_number == 0) {
		throw std::runtime_error("failed to find GPUs with Vulkan support!");
	}

	dprint("Found " << gpu_number << " GPUs:" << std::endl )

	std::vector<VkPhysicalDevice> devices(gpu_number);
	vkEnumeratePhysicalDevices(this->instance, &gpu_number, devices.data());

	struct MyDevice {
		VkPhysicalDevice device;
		VkPhysicalDeviceType type;
		uint32_t score;
	};

	std::vector<MyDevice> devices_suitable;
	devices_suitable.reserve(gpu_number);

	for (const auto& device: devices) {
		VkPhysicalDeviceProperties deviceProperties;
		vkGetPhysicalDeviceProperties(device, &deviceProperties);

		VkPhysicalDeviceFeatures deviceFeatures;
		vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

		VkPhysicalDeviceMemoryProperties memoryProperties;
		vkGetPhysicalDeviceMemoryProperties(device, &memoryProperties);

		dprint( '\t' << deviceProperties.deviceName << "(" << get_device_type_str(deviceProperties.deviceType) << ")" << std::endl )

		if (deviceProperties.deviceType != VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU
			&& deviceProperties.deviceType != VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
			continue;
		else if (!deviceFeatures.geometryShader)
			continue;
		
		QueueFamilyIndices indices = findQueueFamilies(device);

		if (!indices.graphics_family.has_value())
			continue;
		
		if (!indices.present_family.has_value())
			continue;
		
		if (!this->CheckDeviceExtensionSupport(device))
			continue;

		uint32_t score = 0;

		dprint( "\t\t" << "Memory types: " << std::endl )
		for (uint32_t i=0; i<memoryProperties.memoryTypeCount; i++)
			dprint( "\t\t\t" << "heap index: " << memoryProperties.memoryTypes[i].heapIndex
								<< " flags: " << memoryProperties.memoryTypes[i].propertyFlags << std::endl )
		
		dprint( "\t\t" << "Memory heaps: " << std::endl )
		for (uint32_t i=0; i<memoryProperties.memoryHeapCount; i++)
			dprint( "\t\t\t" << "size: " << (memoryProperties.memoryHeaps[i].size / (1024*1024)) << "MB"
								<< " flags: " << memoryProperties.memoryHeaps[i].flags << std::endl )

		switch (deviceProperties.deviceType) {
			case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU:
				score += 500;
			break;

			case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU:
				score += 1000;
			break;
		}
		
		devices_suitable.push_back( MyDevice{
			.device = device,
			.type = deviceProperties.deviceType,
			.score = score
			} );
	}

	if (devices_suitable.size() == 0)
		throw std::runtime_error("failed to find any suitable GPU!");

	std::optional<MyDevice*> integrated_gpu;
	std::optional<MyDevice*> discrete_gpu;

	for (auto& my_device: devices_suitable) {
		if (my_device.type == VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU)
			integrated_gpu = &my_device;
		else if (my_device.type == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
			discrete_gpu = &my_device;
	}

	if (integrated_gpu.has_value()) {
		this->device = (*integrated_gpu)->device;
		if (discrete_gpu.has_value())
			dprint( "we prefer the integrated GPU instead of the discrete GPU, since our requirement is very low" << std::endl )
	}
	else if (discrete_gpu.has_value())
		this->device = (*discrete_gpu)->device;
	else
		throw std::runtime_error("doesn't have an integrated or discrete GPU!");

	//this->device = (*discrete_gpu)->device; // force discrete gpu for testing
}

bool Vulkan::CheckDeviceExtensionSupport (VkPhysicalDevice device)
{
	uint32_t extensionCount;
	vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

	std::vector<VkExtensionProperties> availableExtensions(extensionCount);
	vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());

#if 0
	dprint( "\t\t" << "Device extensions:" << std::endl )

	for (const auto& extension: availableExtensions) {
		dprint( "\t\t\t" << extension.extensionName << std::endl )
	}
#endif

	bool has_all_extensions = true;

	for (const char *extension: this->device_extensions) {
		auto it = std::find_if(availableExtensions.begin(), availableExtensions.end(),
			[extension] (const VkExtensionProperties& x) -> bool {
				return std::string_view(extension) == std::string_view(x.extensionName);
			} );

		if (it == availableExtensions.end()) {
			has_all_extensions = false;
			break;
		}

		dprint( "\t\tFound device extension " << extension << std::endl )
	}

	return has_all_extensions;
}

Vulkan::QueueFamilyIndices Vulkan::findQueueFamilies (VkPhysicalDevice device)
{
	QueueFamilyIndices indices;

	uint32_t queueFamilyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

	std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

	dprint( "\t\t" << "Family queues:" << std::endl )

	uint32_t i = 0;
	for (const auto& queueFamily: queueFamilies) {
		dprint( "\t\t\t" << "queue[" << i << "]: " << get_family_queues_str(queueFamily.queueFlags) << std::endl )

		if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
			indices.graphics_family = i;
		
		VkBool32 presentSupport = false;
		vkGetPhysicalDeviceSurfaceSupportKHR(device, i, this->surface, &presentSupport);

		if (presentSupport)
			indices.present_family = i;

		i++;
	}

	return indices;
}

void Vulkan::CreateDeviceContext ()
{
	SDL_memset(&(this->device_properties), 0, sizeof(VkPhysicalDeviceProperties));

	vkGetPhysicalDeviceProperties(this->device, &(this->device_properties));
	vkGetPhysicalDeviceMemoryProperties(this->device, &(this->device_memory_info));

	dprint( "Driver Version: " << this->device_properties.driverVersion << std::endl )
	dprint( "Device Name: " << this->device_properties.deviceName << std::endl )
	dprint( "Device Type: " << get_device_type_str(this->device_properties.deviceType) << std::endl )
	dprint( "Vulkan Version: " << ((this->device_properties.apiVersion >> 22) & 0x3FF) << ". "
							  << ((this->device_properties.apiVersion >> 12) & 0x3FF) << ". "
							  << (this->device_properties.apiVersion & 0xFFF)
							  << std::endl )
	dprint( std::endl )


	//Get the Physical GPU Supported Operations
	uint32_t family_number = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(this->device, &family_number, nullptr);
	std::vector<VkQueueFamilyProperties> familyProperties(family_number);
	vkGetPhysicalDeviceQueueFamilyProperties(this->device, &family_number, familyProperties.data());

	vkGetPhysicalDeviceFeatures(this->device, &(this->device_features));

	this->family_indices = this->findQueueFamilies(this->device);

	std::set<uint32_t> uniqueQueueFamilies = {
		*(this->family_indices.graphics_family),
		*(this->family_indices.present_family)
		};

	//Create the Device Context

	std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;

	float queuePriority = 1.0f;

	for (uint32_t queueFamily: uniqueQueueFamilies) {
		VkDeviceQueueCreateInfo deviceQueueInfo = {};

		deviceQueueInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		deviceQueueInfo.queueFamilyIndex = queueFamily;
		deviceQueueInfo.queueCount = 1;
		deviceQueueInfo.pQueuePriorities = &queuePriority;

		queueCreateInfos.push_back(deviceQueueInfo);
	}

	//VkPhysicalDeviceFeatures deviceFeatures{};

	VkDeviceCreateInfo contextInfo = {};
	contextInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	contextInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
	contextInfo.pQueueCreateInfos = queueCreateInfos.data();
	//contextInfo.pEnabledFeatures = &deviceFeatures;
	contextInfo.enabledExtensionCount = this->device_extensions.size();
	contextInfo.ppEnabledExtensionNames = this->device_extensions.data();

	VkResult result = vkCreateDevice(this->device, &contextInfo, nullptr, &(this->device_context));

	if (result != VK_SUCCESS)
		throw std::runtime_error("Could not create a Device Context!");

	vkGetDeviceQueue(this->device_context, *(this->family_indices.graphics_family), 0, &(this->graphics_queue));
	vkGetDeviceQueue(this->device_context,  *(this->family_indices.present_family), 0, &(this->present_queue));
}

void Vulkan::QuerySurface ()
{
	VkBool32 WSI_supported = false;
	VkResult result = vkGetPhysicalDeviceSurfaceSupportKHR(this->device, *(this->family_indices.graphics_family), this->surface, &WSI_supported);
	if (!WSI_supported)
		throw std::runtime_error("Could not not create window surface for Vulkan!");

	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(this->device, this->surface, &(this->surface_caps));

	// get actual scren size

	if (this->surface_caps.currentExtent.width != std::numeric_limits<uint32_t>::max())
		this->extent = this->surface_caps.currentExtent;
	else {
		this->extent.width = std::clamp(this->screen_width, this->surface_caps.minImageExtent.width, this->surface_caps.maxImageExtent.width);
		this->extent.height = std::clamp(this->screen_height, this->surface_caps.minImageExtent.height, this->surface_caps.maxImageExtent.height);
	}

	dprint( "screen size set to " << this->extent.width << "x" << this->extent.height << std::endl )

	// get supported formats

	uint32_t format_count = 0;
	vkGetPhysicalDeviceSurfaceFormatsKHR(this->device, this->surface, &format_count, nullptr);

	if (format_count == 0)
		throw std::runtime_error("Could not get surface information for Vulkan Swapchain!");

	std::vector<VkSurfaceFormatKHR> formats(format_count);
	vkGetPhysicalDeviceSurfaceFormatsKHR(this->device, this->surface, &format_count, formats.data());

	// get supported vsync modes

	uint32_t presentModeCount;
	vkGetPhysicalDeviceSurfacePresentModesKHR(this->device, this->surface, &presentModeCount, nullptr);

	if (presentModeCount == 0)
		throw std::runtime_error("Could not get surface information (pre) for Vulkan Swapchain!");
	
	std::vector<VkPresentModeKHR> presentModes(presentModeCount);
	vkGetPhysicalDeviceSurfacePresentModesKHR(this->device, this->surface, &presentModeCount, presentModes.data());

	// chose format

	std::optional<VkSurfaceFormatKHR> best_format;

	for (const auto& format: formats) {
		if (format.format == VK_FORMAT_B8G8R8A8_SRGB && format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
			best_format = format;
			break;
		}
	}

	if (best_format.has_value())
		this->surface_format = *best_format;
	else
		this->surface_format = formats[0];
	
	// chose vsync mode

	this->present_mode = VK_PRESENT_MODE_FIFO_KHR; // default/fallback mode

	for (const auto& mode : presentModes) {
		if (mode == VK_PRESENT_MODE_MAILBOX_KHR) { // most desiared mode
			this->present_mode = mode;
			break;
		}
	}
}

void Vulkan::CreateSwapchain ()
{
	//this->swapchain_buffer_count = 2;
	this->swapchain_buffer_count = this->surface_caps.minImageCount + 1;

	if (this->surface_caps.maxImageCount > 0 && this->swapchain_buffer_count > this->surface_caps.maxImageCount)
		this->swapchain_buffer_count = this->surface_caps.maxImageCount;

	VkSwapchainCreateInfoKHR swapchain_info {};
	swapchain_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	swapchain_info.surface = this->surface;
	swapchain_info.minImageCount = this->swapchain_buffer_count;
	swapchain_info.imageFormat = this->surface_format.format;
	swapchain_info.imageColorSpace = this->surface_format.colorSpace;
	swapchain_info.imageExtent = this->extent;
	swapchain_info.imageArrayLayers = 1;
	swapchain_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

	if (this->family_indices.graphics_family != this->family_indices.present_family) {
		uint32_t queueFamilyIndices[] = {
			*(this->family_indices.graphics_family),
			*(this->family_indices.present_family)
		};

		swapchain_info.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
		swapchain_info.queueFamilyIndexCount = 2;
		swapchain_info.pQueueFamilyIndices = queueFamilyIndices;
	} else {
		swapchain_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		swapchain_info.queueFamilyIndexCount = 0; // Optional
		swapchain_info.pQueueFamilyIndices = nullptr; // Optional
	}

	swapchain_info.preTransform = this->surface_caps.currentTransform; //VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
	swapchain_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	swapchain_info.presentMode = this->present_mode;
	swapchain_info.clipped = VK_TRUE;
	swapchain_info.oldSwapchain = VK_NULL_HANDLE; //old_swapchain;

	if (vkCreateSwapchainKHR(this->device_context, &swapchain_info, nullptr, &(this->swapchain)) != VK_SUCCESS)
		throw std::runtime_error("failed to create swap chain!");
}

void Vulkan::CreateSwapchainImages ()
{
	vkGetSwapchainImagesKHR(this->device_context, this->swapchain, &(this->swapchain_buffer_count), nullptr);

	this->swapchain_buffers.resize(this->swapchain_buffer_count);
	this->swapchain_buffer_view.resize(this->swapchain_buffer_count);

	vkGetSwapchainImagesKHR(this->device_context, this->swapchain, &(this->swapchain_buffer_count), this->swapchain_buffers.data());

	for (uint32_t i = 0; i < this->swapchain_buffer_count; i++) {
		VkImageViewCreateInfo swapchain_view_info {};
		swapchain_view_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		swapchain_view_info.image = this->swapchain_buffers[i];
		swapchain_view_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
		swapchain_view_info.format = this->surface_format.format;
		swapchain_view_info.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
		swapchain_view_info.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
		swapchain_view_info.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
		swapchain_view_info.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
		swapchain_view_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		swapchain_view_info.subresourceRange.baseMipLevel = 0;
		swapchain_view_info.subresourceRange.levelCount = 1;
		swapchain_view_info.subresourceRange.baseArrayLayer = 0;
		swapchain_view_info.subresourceRange.layerCount = 1;

		if (vkCreateImageView(this->device_context, &swapchain_view_info, nullptr, &(this->swapchain_buffer_view[i])) != VK_SUCCESS)
			throw std::runtime_error("failed to create image views!");
	}
}

#if 0
void Vulkan::CreateDepthStencilImage() //Create the Depth and Stencil Buffer Attachments for Swapchain Rendering
{
	//Check for the format of the Depth/Stencil Buffer
	vector<VkFormat> depth_formats {VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT,VK_FORMAT_D16_UNORM_S8_UINT, VK_FORMAT_D32_SFLOAT, VK_FORMAT_D16_UNORM};
	for (auto format : depth_formats) {
		VkFormatProperties format_properties{};
		vkGetPhysicalDeviceFormatProperties(device, format, &format_properties);
		if (format_properties.optimalTilingFeatures &VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT) {
			depth_buffer_format = format;
			break;
		}
	}

	if (depth_buffer_format == VK_FORMAT_UNDEFINED) {
		SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_ERROR, "Couldn't get depth buffer format!");
	}

	//Check the Buffer Aspect
	if ((depth_buffer_format == VK_FORMAT_D32_SFLOAT_S8_UINT) ||
		(depth_buffer_format == VK_FORMAT_D24_UNORM_S8_UINT) ||
		(depth_buffer_format == VK_FORMAT_D16_UNORM_S8_UINT) ||
		(depth_buffer_format == VK_FORMAT_S8_UINT)) {
		stencil_support = true;
	}

	//Create Depth Buffer
	VkImageCreateInfo image_create_info {};
	image_create_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	image_create_info.flags = 0;
	image_create_info.imageType = VK_IMAGE_TYPE_2D;
	image_create_info.format = depth_buffer_format;
	image_create_info.extent.width = render_width;
	image_create_info.extent.height = render_height;
	image_create_info.extent.depth = 1;
	image_create_info.mipLevels = 1;
	image_create_info.arrayLayers = 1;
	image_create_info.samples = VK_SAMPLE_COUNT_1_BIT;
	image_create_info.tiling = VK_IMAGE_TILING_OPTIMAL;
	image_create_info.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
	image_create_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	image_create_info.queueFamilyIndexCount = VK_QUEUE_FAMILY_IGNORED;
	image_create_info.pQueueFamilyIndices = nullptr;
	image_create_info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

	vkCreateImage(device_context, &image_create_info, nullptr, &depth_stencil_buffer);

	VkMemoryRequirements buffer_memory_req; //Get the required information about memory for the buffer
	vkGetImageMemoryRequirements(device_context, depth_stencil_buffer, &buffer_memory_req);

	uint32_t memory_index = UINT_FAST32_MAX;  //Index of GPU memory location
	auto propeties = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT; //application controlled property of the memory used with the GPU
	for (uint32_t i = 0; i < device_memory_info.memoryTypeCount; i++) {
		if (buffer_memory_req.memoryTypeBits & (1 << i)) {
			if ((device_memory_info.memoryTypes[i].propertyFlags & propeties) == propeties) {
				memory_index = i;
				break;
			}
		}
	}
	SDL_assert(memory_index != UINT32_MAX);

	VkMemoryAllocateInfo memory_info{}; //info about the buffer's memory
	memory_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	memory_info.allocationSize = buffer_memory_req.size;
	memory_info.memoryTypeIndex = memory_index;

	vkAllocateMemory(device_context, &memory_info, nullptr, &depth_stencil_buffer_memory); //allocate memory for the Depth/Stencil Buffer
	vkBindImageMemory(device_context, depth_stencil_buffer, depth_stencil_buffer_memory, 0); //Bind the Depth/Stencil Buffer

	VkImageViewCreateInfo image_view_info {};
	image_view_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	image_view_info.image = depth_stencil_buffer;
	image_view_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
	image_view_info.format = depth_buffer_format;
	image_view_info.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
	image_view_info.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
	image_view_info.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
	image_view_info.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
	image_view_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT | (stencil_support ? VK_IMAGE_ASPECT_STENCIL_BIT : 0);
	image_view_info.subresourceRange.baseMipLevel = 0;
	image_view_info.subresourceRange.levelCount = 1;
	image_view_info.subresourceRange.baseArrayLayer = 0;
	image_view_info.subresourceRange.layerCount = 1;

	vkCreateImageView(device_context, &image_view_info, nullptr, &depth_stencil_buffer_view);
}

void Vulkan::DestroyDepthStencilImage()
{
	vkDestroyImageView(device_context, depth_stencil_buffer_view, nullptr);
	vkFreeMemory(device_context, depth_stencil_buffer_memory, nullptr);
	vkDestroyImage(device_context, depth_stencil_buffer, nullptr);
}
#endif

void Vulkan::CreateRenderPass ()
{
	//VkFormat depth_buffer_format = VK_FORMAT_UNDEFINED;

	std::array<VkAttachmentDescription, 1> attachments {};

/*	attachments[0].flags = 0;
	attachments[0].format = depth_buffer_format;
	attachments[0].samples = VK_SAMPLE_COUNT_1_BIT;
	attachments[0].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	attachments[0].storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	attachments[0].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	attachments[0].stencilStoreOp = VK_ATTACHMENT_STORE_OP_STORE;
	attachments[0].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	attachments[0].finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
	*/

	attachments[0].flags = 0;
	attachments[0].format = this->surface_format.format;
	attachments[0].samples = VK_SAMPLE_COUNT_1_BIT;
	attachments[0].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	attachments[0].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	attachments[0].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	attachments[0].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	attachments[0].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	attachments[0].finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

	// create subpasses

	std::array<VkAttachmentReference, 1> sub_pass_color_attachments {}; // synonymous with (in glsl): layout(location = 0) out vec4 FinalColor

	sub_pass_color_attachments[0].attachment = 0;
	sub_pass_color_attachments[0].layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	std::array<VkSubpassDescription, 1> sub_passes { };
	
	sub_passes[0].flags = 0;
	sub_passes[0].pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
//		sub_passes[0].inputAttachmentCount = 0;
//		sub_passes[0].pInputAttachments = nullptr;
	sub_passes[0].colorAttachmentCount = sub_pass_color_attachments.size();
	sub_passes[0].pColorAttachments = sub_pass_color_attachments.data();
/*	sub_passes[0].pResolveAttachments = nullptr;
	sub_passes[0].pDepthStencilAttachment = &sub_pass_depth_attachment;
	sub_passes[0].preserveAttachmentCount = 0;
	sub_passes[0].pPreserveAttachments = nullptr;*/

	std::array<VkSubpassDependency, 1> dependencies { };
	dependencies[0].srcSubpass = VK_SUBPASS_EXTERNAL;
	dependencies[0].dstSubpass = 0;
	dependencies[0].srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependencies[0].srcAccessMask = 0;
	dependencies[0].dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependencies[0].dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

	VkRenderPassCreateInfo render_pass_info { };
	render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	render_pass_info.attachmentCount = attachments.size();
	render_pass_info.pAttachments = attachments.data();
	render_pass_info.subpassCount = sub_passes.size();
	render_pass_info.pSubpasses = sub_passes.data();
	render_pass_info.dependencyCount = dependencies.size();
	render_pass_info.pDependencies = dependencies.data();

	if (vkCreateRenderPass(device_context, &render_pass_info, nullptr, &render_pass) != VK_SUCCESS)
		throw std::runtime_error("failed to create render pass!");
}

// create the framebuffers (which bind the image attachments to the renderpass)

void Vulkan::CreateFramebuffers ()
{
	this->framebuffers.resize(this->swapchain_buffer_count);
	for (uint32_t i = 0; i < this->swapchain_buffer_count; i++) {
		std::array<VkImageView, 1> attachments {};

		attachments[0] = this->swapchain_buffer_view[i];
		
		VkFramebufferCreateInfo framebuffer_info {};
		framebuffer_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebuffer_info.renderPass = this->render_pass;
		framebuffer_info.attachmentCount = attachments.size();
		framebuffer_info.pAttachments = attachments.data();
		framebuffer_info.width = this->extent.width;
		framebuffer_info.height = this->extent.height;
		framebuffer_info.layers = 1;

		if (vkCreateFramebuffer(this->device_context, &framebuffer_info, nullptr, &(this->framebuffers[i])) != VK_SUCCESS)
			throw std::runtime_error("failed to create framebuffer!");
	}
}

void Vulkan::StartSynchronizations ()
{
	VkSemaphoreCreateInfo semaphore_info = { };
	semaphore_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
	
	if (vkCreateSemaphore(this->device_context, &semaphore_info, nullptr, &(this->sem_present)) != VK_SUCCESS)
		throw std::runtime_error("failed to create semaphore present!");
	
	if (vkCreateSemaphore(this->device_context, &semaphore_info, nullptr, &(this->sem_render)) != VK_SUCCESS)
		throw std::runtime_error("failed to create semaphore render!");

	VkFenceCreateInfo fence_info = { };
	fence_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fence_info.flags = VK_FENCE_CREATE_SIGNALED_BIT;

	if (vkCreateFence(this->device_context, &fence_info, VK_NULL_HANDLE, &(this->in_flight_fence)) != VK_SUCCESS)
		throw std::runtime_error("failed to create fence!");
	
	/*swapchain_available.resize(swapchain_buffer_count);
	for (int i = 0; i < swapchain_buffer_count; i++){
		vkCreateFence(device_context, &fence_info, VK_NULL_HANDLE, &swapchain_available[i]);
	}*/	
}

void Vulkan::AcquireNextSwapchain ()
{
	vkWaitForFences(this->device_context, 1, &this->in_flight_fence, VK_TRUE, std::numeric_limits<uint64_t>::max());
	vkResetFences(this->device_context, 1, &this->in_flight_fence);
	
	VkResult result = vkAcquireNextImageKHR(
		this->device_context,
		this->swapchain,
		std::numeric_limits<uint64_t>::max(),
		this->sem_present,
		nullptr,
		&(this->active_swapchain_id)
		);
}

void Vulkan::BeginRenderPresent (VkCommandBuffer& command_buffer)
{
	// Submit Command Buffers
	VkSubmitInfo submit_info { };
	submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

	VkPipelineStageFlags flags = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	
	std::array<VkSemaphore, 1> waitSemaphores;
	waitSemaphores[0] = this->sem_present;
	submit_info.waitSemaphoreCount = waitSemaphores.size();
	submit_info.pWaitSemaphores = waitSemaphores.data();
	submit_info.pWaitDstStageMask = &flags;

	submit_info.commandBufferCount = 1;
	submit_info.pCommandBuffers = &command_buffer;

	std::array<VkSemaphore, 1> signalSemaphores;
	signalSemaphores[0] = this->sem_render;
	submit_info.signalSemaphoreCount = signalSemaphores.size();
	submit_info.pSignalSemaphores = signalSemaphores.data();

	if (vkQueueSubmit(this->graphics_queue, 1, &submit_info, this->in_flight_fence) != VK_SUCCESS)
		throw std::runtime_error("failed to submit draw command buffer!");

	VkPresentInfoKHR present_info { };
	present_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	present_info.waitSemaphoreCount = signalSemaphores.size();
	present_info.pWaitSemaphores = signalSemaphores.data();
	present_info.swapchainCount = 1;
	present_info.pSwapchains = &this->swapchain;
	present_info.pImageIndices = &this->active_swapchain_id;
	present_info.pResults = nullptr;

	vkQueuePresentKHR(this->present_queue, &present_info);
}

//~Rendering Functions



//---------------------------------------------VK_VALIDATION_LAYER_CODE--------------------------------------------------------
#if 0
#ifdef VK_DEBUG
PFN_vkCreateDebugReportCallbackEXT fvkCreateDebugReportCallbackEXT = nullptr;
PFN_vkDestroyDebugReportCallbackEXT fvkDestroyDebugReportCallbackEXT = nullptr;

VKAPI_ATTR VkBool32 VKAPI_CALL VulkanDebugCall(
	VkDebugReportFlagsEXT       flags,
	VkDebugReportObjectTypeEXT  obj_type,
	uint64_t                    src_object,
	size_t                      location,
	int32_t                     msg_code,
	const char*                 layer_prefix,
	const char*                 msg,
	void *                      user_data)
{
	string stream = msg;
	/*
	if (stream.length() > 100)
		stream.resize(120);
	*/

	if (flags & VK_DEBUG_REPORT_INFORMATION_BIT_EXT) {
		SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_INFO, msg);
		printf("\n");
		return false;
	}
	else if (flags & VK_DEBUG_REPORT_WARNING_BIT_EXT | VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT) {
		SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_WARN, msg);
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_WARNING, "Vulkan Warning!", stream.c_str(), NULL);
		printf("\n");
		return false;
	}
	else if (flags & VK_DEBUG_REPORT_ERROR_BIT_EXT) {
		SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_ERROR, msg);
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Vulkan Error!", stream.c_str(), NULL);
		printf("\n");
		return true;
	}

	else {
		return false;
	}

}

void Vulkan::SetupDebug()
{

	debug_create_info.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CREATE_INFO_EXT;
	debug_create_info.pfnCallback = VulkanDebugCall;
	debug_create_info.flags =
		VK_DEBUG_REPORT_INFORMATION_BIT_EXT |
		VK_DEBUG_REPORT_WARNING_BIT_EXT |
		VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT |
		VK_DEBUG_REPORT_ERROR_BIT_EXT |
		0;

	instance_layers.push_back("VK_LAYER_LUNARG_standard_validation");
	instance_extensions.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
}

void Vulkan::InitDebug()
{
	fvkCreateDebugReportCallbackEXT = (PFN_vkCreateDebugReportCallbackEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugReportCallbackEXT");
	fvkDestroyDebugReportCallbackEXT = (PFN_vkDestroyDebugReportCallbackEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugReportCallbackEXT");
	if (fvkCreateDebugReportCallbackEXT == nullptr || fvkDestroyDebugReportCallbackEXT == nullptr) {
		SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_ERROR, "Could not get debug functions!");
	}

	result = fvkCreateDebugReportCallbackEXT(instance, &debug_create_info, nullptr, &debug_report);
}

void Vulkan::DestroyDebug()
{
	fvkDestroyDebugReportCallbackEXT(instance, debug_report, nullptr);
}

#endif
//-------------------------------------------------------------------------------------------------------------------------------
#endif