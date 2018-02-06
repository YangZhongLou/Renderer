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
	struct Vertex;
	
	class Renderer
	{
	private:
		UInt32 m_currentBuffer = 0;
		std::vector<VkCommandBuffer> m_drawCmdBuffers;
		
		Device * m_device;
		Debugger * m_debugger;
		
		
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
		std::vector<VkViewport> m_viewports;
		
		Swapchain * m_swapchain;

		Camera m_camera;

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
		void SetViewports(std::vector<VkViewport> & viewports);
		void SetPipelines(std::vector<VkPipeline> & pipelines);
		
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
		void InitVulkanDevice();
		void InitVulkanDebugger();
		void InitSwapchain();
		void InitCommandBuffers();

		void InitVeritces();
		void InitViewports();
		void InitPipelineCache();
		
		void InitDepthStencil();
		void InitFramebuffers();
		void InitRenderPass();
		void InitPipelines();
		
		void InitUniforms();
	};
}