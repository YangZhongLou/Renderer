#include "renderer.h"
#include "device.h"
#include "vk_factory.hpp"
#include "utils.h"
#include "uniforms.h"
#include "swapchain.h"
#include "vertices.h"
#include "debugger.h"
#include "shader.h"
#include "keycodes.hpp"
#include "vk_instance.h"
#include "defines.h"
#include "synchronizations.h"

namespace Concise
{
	Renderer::Renderer()
	{
		m_swapchain = nullptr;
		
		m_vertices = new Vertices();
		m_uniforms = new Uniforms();
	}
	
	Renderer::~Renderer()
	{
		SAFE_DELETE(m_uniforms);
		SAFE_DELETE(m_vertices);
		SAFE_DELETE(m_swapchain);
	}

	void Renderer::Loop()
	{
		MSG msg;
		bool quitMessageReceived = false;
		while (!quitMessageReceived) {
			while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
				TranslateMessage(&msg);
				DispatchMessage(&msg);
				if (msg.message == WM_QUIT) {
					quitMessageReceived = true;
					break;
				}
			}

			RenderFrame();
		}

		vkDeviceWaitIdle(Device::Instance().GetLogicalDevice());
	}
	
	void Renderer::SubmitVerticesData(std::vector<Vertex> & verticesData, std::vector<UInt32> & indicesData)
	{
		m_vertices->Submit(verticesData, indicesData);
	}
	
	
	void Renderer::RenderFrame()
	{
		VK_CHECK_RESULT(m_swapchain->AcquireNextImage(m_presentCompleteSemaphore, &m_currentBuffer));

		VK_CHECK_RESULT(vkWaitForFences(m_device->GetLogicalDevice(), 1, &m_fences[m_currentBuffer], VK_TRUE, UINT64_MAX));
		VK_CHECK_RESULT(vkResetFences(m_device->GetLogicalDevice(), 1, &m_fences[m_currentBuffer]));

		VkPipelineStageFlags waitStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		
		VkSubmitInfo submitInfo = VkFactory::SubmitInfo();
		submitInfo.pWaitDstStageMask = &waitStageMask;									
		submitInfo.pWaitSemaphores = &m_presentCompleteSemaphore;							
		submitInfo.waitSemaphoreCount = 1;																														
		submitInfo.pSignalSemaphores = &m_renderCompleteSemaphore;						
		submitInfo.signalSemaphoreCount = 1;											
		submitInfo.pCommandBuffers = &m_drawCmdBuffers[m_currentBuffer];					
		submitInfo.commandBufferCount = 1;												

		VK_CHECK_RESULT(vkQueueSubmit(m_device->GetQueue(), 1, &submitInfo, m_fences[m_currentBuffer]));
		
		VK_CHECK_RESULT(m_swapchain->QueuePresent(m_device->GetQueue(), m_currentBuffer, m_renderCompleteSemaphore));

		VK_CHECK_RESULT(vkQueueWaitIdle(m_device->GetQueue()));
	}
}