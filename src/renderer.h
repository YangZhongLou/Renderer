/*
* This code is licensed under the MIT license (MIT) (http://opensource.org/licenses/MIT)
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
	struct Vertex;
	
	class Renderer
	{
	private:
		VkSemaphore m_presentCompleteSemaphore;
		VkSemaphore m_renderCompleteSemaphore;
		std::vector<VkFence> m_fences;
		
		UInt32 m_currentBuffer = 0;
		std::vector<VkCommandBuffer> m_drawCmdBuffers;
		
		Device * m_device;
		Debugger * m_debugger;
		
		VkDescriptorPool m_descriptorPool = VK_NULL_HANDLE;
		VkDescriptorSetLayout m_descriptorSetLayout;
		VkDescriptorSet m_descriptorSet;
		
		VkPipelineLayout m_pipelineLayout;
		VkPipeline m_pipeline;
		VkPipelineCache m_pipelineCache;
		
		Vertices * m_vertices;
		Uniforms * m_uniforms;
		struct
		{
			VkImage image;
			VkDeviceMemory mem;
			VkImageView view;
		} m_depthStencil;
		VkRenderPass m_renderPass;
		std::vector<VkFramebuffer> m_framebuffers;
		Swapchain * m_swapchain;
		
		/** refactor */
		UInt32 m_destWidth;
		UInt32 m_destHeight;
		bool m_resizing;

		/** refactor */
		bool m_prepared = false;
		bool m_paused;
		UInt32 m_width = 1280;
		UInt32 m_height = 720;
		float m_zoom = -2.5;

		float m_rotationSpeed = 1.0f;
		float m_zoomSpeed = 1.0f;
		bool m_viewUpdated;

		glm::vec3 m_rotation = glm::vec3();
		glm::vec3 m_cameraPos = glm::vec3();
		glm::vec2 m_mousePos;

		struct 
		{
			bool left = false;
			bool right = false;
			bool middle = false;
		} m_mouseButtons;

		Camera m_camera;

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

		void InitConsole(std::string title);
		HWND BuildWindow(HINSTANCE hinstance, WNDPROC wndproc, std::string name = "", std::string windowTitle = "");
		void HandleMessages(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
		void KeyPressed(UInt32) {}
		void HandleMouseMove(Int32 x, Int32 y);
		void MouseMoved(double x, double y, bool handled) {}
		void WindowResize();
		void WindowResized();
		void ViewChanged();

		void DestroyCommandBuffers();
		void CreateCommandBuffers();
		void BuildCommandBuffers();
		void Loop();
		void RenderFrame();
		
		void SubmitVerticesData(std::vector<Vertex> & verticesData, std::vector<UInt32> & indicesData);

		UInt32 GetWidth() const { return m_width; }
		UInt32 GetHeight() const { return m_height; }
		float GetZoom() const { return m_zoom; };
		glm::vec3 GetRotation() const { return m_rotation; }

		bool IsPrepared() const { return m_prepared; }

#if defined(_WIN32)
		HWND GetWindowHandle() const { return m_window; }
		HINSTANCE GetWindowInstance() const { return m_windowInstance; }
#endif

	private:
		void InitVulkan();
		void InitVulkanInstance(bool enableValidation);
		void InitVulkanDevice();
		void InitVulkanDebugger();
		void InitSwapchain();
		void InitCommandBuffers();
		void InitVulkanSync();

		void InitVeritces();

		void InitPipelineCache();
		
		void InitDepthStencil();
		void InitFramebuffers();
		void InitRenderPass();
		void InitPipelines();
		
		void InitUniforms();
		void InitDescriptorPool();
		/** refactor this later */
		void InitDescriptorSetLayout();
		void InitDescriptorSet();
	};
}