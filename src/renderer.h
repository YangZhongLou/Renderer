/**
* Author ZhongLou Yang, 2018-2-5
*/
#pragma once

#include <vulkan/vulkan.h>
#include <vector>
#include "types.h"
#include "camera.hpp"

namespace Concise
{
	class Device;
	class Vertices;
	class Uniforms;
	class Swapchain;
	class Debugger;
	class VulkanInstance;

	struct Vertex;
	
	class Renderer
	{
	private:
		UInt32 m_currentBuffer = 0;
		std::vector<VkCommandBuffer> m_drawCmdBuffers;
		
		VulkanInstance * m_vkInstance;
		Device * m_device;
		Debugger * m_debugger;
		
		Vertices * m_vertices;
		Uniforms * m_uniforms;
		
		VkRenderPass m_renderPass;
		std::vector<VkFramebuffer> m_framebuffers;
		std::vector<VkViewport> m_viewports;
		
		Swapchain * m_swapchain;

		Camera m_camera;
	public:
		Renderer();
		~Renderer();
	public:
		/** refactor this later */

		void WindowResize();
		void WindowResized();
		void ViewChanged();

		void DestroyCommandBuffers();
		void CreateCommandBuffers();
		void BuildCommandBuffers();
		void Loop();
		void RenderFrame();
		
		void SubmitVerticesData(std::vector<Vertex> & verticesData, std::vector<UInt32> & indicesData);
		void SetViewports(std::vector<VkViewport> & viewports);
		void SetPipelines(std::vector<VkPipeline> & pipelines);
	};
}