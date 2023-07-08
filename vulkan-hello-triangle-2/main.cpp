#include <vector>
#include <array>
#include <iostream>

#include <cmath>

#include "Renderer.h"
#include "pipeline.h"
#include "Helper.h"

constexpr double PI = 3.14159265358979323846;
constexpr double CIRCLE_RAD = PI * 2;
constexpr double CIRCLE_THIRD = CIRCLE_RAD / 3.0;
constexpr double CIRCLE_THIRD_1 = 0;
constexpr double CIRCLE_THIRD_2 = CIRCLE_THIRD;
constexpr double CIRCLE_THIRD_3 = CIRCLE_THIRD * 2;

int main(int argc, char** argv) // Equivalent to WinMain, this initializes SDL2
{
	
	SDL_Init(SDL_INIT_VIDEO);   // This activates the specified SDL2 subsystem;

	//Forward Declarations
	SDL_Window * window;    //  window handle
	SDL_Event event;		//   event handle
	int WIDTH = 640, HEIGHT = 480;

	window = SDL_CreateWindow("Hello Vulkan", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
		WIDTH, HEIGHT, SDL_WINDOW_VULKAN|SDL_WINDOW_ALLOW_HIGHDPI);
	
	if (window == NULL){
		dprint( SDL_GetError() );
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_WARNING, "Window Error", SDL_GetError(), NULL);
		return -1;
	}

	Vulkan renderer(window, WIDTH, HEIGHT);    //  vulkan renderer handle
	VulkanPipeline pipeline(renderer);

	//Variables
	float deltaTime;
	bool running = true;
	float rotator = 0.0f;

	//Rendering
	VkCommandPool command_pool = VK_NULL_HANDLE;

	VkCommandPoolCreateInfo pool_create_info{};
	pool_create_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	pool_create_info.flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT|VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
	pool_create_info.queueFamilyIndex = renderer.get_graphics_family();

	if (vkCreateCommandPool(renderer.device_context, &pool_create_info, nullptr, &command_pool) != VK_SUCCESS)
		throw std::runtime_error("failed to create command pool!");

	VkCommandBuffer command_buffer;

	VkCommandBufferAllocateInfo command_buffer_info {};
	command_buffer_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	command_buffer_info.pNext = nullptr;
	command_buffer_info.commandPool = command_pool;
	command_buffer_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	command_buffer_info.commandBufferCount = 1;

	if (vkAllocateCommandBuffers(renderer.device_context, &command_buffer_info, &command_buffer) != VK_SUCCESS)
		throw std::runtime_error("failed to allocate command buffers!");

	//FPS Defines
	double lastTime = SDL_GetTicks() / 1000;
	float lastFrame = SDL_GetTicks();
	int nmbf = 0;
	bool PrintFPS = true;

	while (running) {
		//Acquire the right swapchain to be used for rendering. the swapchain buffer index is used for rendering
		renderer.AcquireNextSwapchain();
		vkResetCommandBuffer(command_buffer, 0);

		//Game Logic
		if (PrintFPS == true) {
			double currentTime = SDL_GetTicks() / 1000;
			nmbf++;
			if (currentTime - lastTime >= 1.0) {
				printf("\n Framerate: %f \n", 1.0 * (double)nmbf);
				nmbf = 0;
				lastTime += 1.0;
			}
		}

		// per frame calculations
		float currentFrame = SDL_GetTicks();
		deltaTime = (currentFrame - lastFrame) / 1000;
		lastFrame = currentFrame;

		//OS Events
		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT) {
				running = false;
				break;
			}
		}

		//Record to Command Buffers
		VkCommandBufferBeginInfo command_buffer_begin_info {};
		command_buffer_begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		command_buffer_begin_info.flags = 0; //VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

		if (vkBeginCommandBuffer(command_buffer, &command_buffer_begin_info) != VK_SUCCESS)
			throw std::runtime_error("failed to begin recording command buffer!");

		//sending info to the render pass as to how it renders (clear and depth color, the framebuffer that the drawing commands are applied to)
		VkRect2D render_area {};
		render_area.offset = { 0, 0 };
		render_area.extent = renderer.extent;

		//rotator += 0.001;

		std::array<VkClearValue, 1> color_values {};
		color_values[0] = {{{0.0f, 0.0f, 0.0f, 1.0f}}};

		VkRenderPassBeginInfo render_pass_begin_info{};
		render_pass_begin_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		render_pass_begin_info.renderPass = renderer.render_pass;
		render_pass_begin_info.framebuffer = renderer.framebuffers[renderer.active_swapchain_id];
		render_pass_begin_info.renderArea = render_area;
		render_pass_begin_info.clearValueCount = color_values.size();
		render_pass_begin_info.pClearValues = color_values.data();

		vkCmdBeginRenderPass(command_buffer, &render_pass_begin_info, VK_SUBPASS_CONTENTS_INLINE);
		//at this point drawing commands can begin...

		vkCmdBindPipeline(command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline.graphics_pipeline);

		VkViewport viewport {};
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = static_cast<float>(renderer.extent.width);
		viewport.height = static_cast<float>(renderer.extent.height);
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;

		vkCmdSetViewport(command_buffer, 0, 1, &viewport);

		VkRect2D scissor {};
		scissor.offset = { 0, 0 };
		scissor.extent = renderer.extent;

		vkCmdSetScissor(command_buffer, 0, 1, &scissor);

		vkCmdDraw(command_buffer, 3, 1, 0, 0);

		//...up until this point
		vkCmdEndRenderPass(command_buffer);

		if (vkEndCommandBuffer(command_buffer) != VK_SUCCESS)
			throw std::runtime_error("failed to record command buffer!");

		//Start the rendering stage, and begin the presentation stage for the swapchain when the buffer is finished rendering.
		renderer.BeginRenderPresent(command_buffer);
	}

	vkQueueWaitIdle(renderer.graphics_queue);
	vkQueueWaitIdle(renderer.present_queue);
	vkDestroyCommandPool(renderer.device_context, command_pool, nullptr);

	SDL_DestroyWindow(window);
	SDL_Quit();

	return 0;
}
