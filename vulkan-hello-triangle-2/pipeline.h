#pragma once

#include <vulkan/vulkan.h>
//SDL2
#include <SDL2/SDL.h>
#include <SDL2/SDL_vulkan.h>

#include "Renderer.h"

class VulkanShader
{
	friend class VulkanPipeline;

private:
	VkShaderModule shader_module;
	Vulkan& renderer;

public:
	VulkanShader (Vulkan& renderer_, const char *fname_);
	~VulkanShader ();
};

class VulkanPipeline
{
private:
	Vulkan& renderer;
	VulkanShader vertex_shader;
	VulkanShader frag_shader;
	VkPipelineLayout pipeline_layout;

public:
	VulkanPipeline (Vulkan& renderer_);
	~VulkanPipeline ();
};