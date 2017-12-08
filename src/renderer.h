/*
* This code is licensed under the MIT license (MIT) (http://opensource.org/licenses/MIT)
*/
#pragma once

#include <vulkan/vulkan.h>
#include <vector>
#include "types.h"

namespace Concise
{
	class Device;
	class Vertices;
	class Uniforms;
	class Swapchain;
	
	class Renderer
	{
	private:
		VkSemaphoreCreateInfo m_presentSemaphore;
		VkSemaphoreCreateInfo m_renderSemaphore;
		std::vector<VkFence> m_fences;
		
		std::vector<VkCommandBuffer> m_drawCommandBuffers;
		
		Device * m_device;
		
		UInt32 m_width;
		UInt32 m_Height;
		
		VkDescriptorPool m_descriptorPool = VK_NULL_HANDLE;
		VkDescriptorSetLayout m_descriptorSetLayout;
		VkDescriptorSet m_descriptorSet;
		
		VkPipelineLayout m_pipelineLayout;
		VkPipeline m_pipeline;
		
		Vertices * m_vertices;
		Uniforms * m_uniforms;
		VkFormat m_depthFormat;
		struct
		{
			VkImage image;
			VkDeviceMemory mem;
			VkImageView view;
		} m_depthStencil;
		VkRenderPass m_renderPass;
		std::vector<VkFramebuffer> m_framebuffers;
		Swapchain * m_swapchain;

		struct Settings
		{
			bool validation = false;
			bool fullscreen = false;
			bool vsync = false;
			bool overlay = false;
		} m_settings;
		
		VkInstance m_vkInstance;
		
#if defined(_WIN32)
		HWND m_window;
		HINSTANCE m_windowInstance;
#endif

	public:
		Renderer();
		~Renderer();
	public:
		void Init();
		/** refactor this later */
		void InitWindow(HINSTANCE hinstance, WNDPROC wndproc);
		
		void BuildCommandBuffers();
	private:
		void InitSync();
		void InitVulkanInstance(bool enableValidation);
		
		void InitDepthStencil();
		void InitFramebuffers();
		void InitRenderPass();
		
		void InitDescriptorPool();
		/** refactor this later */
		void InitDescriptorSetLayout();
		void InitDescriptorSet();
	}
}