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
	
	void Renderer::DestroyCommandBuffers()
	{
		vkFreeCommandBuffers(m_device->GetLogicalDevice(), m_device->GetCommandPool(), static_cast<UInt32>(m_drawCmdBuffers.size()), m_drawCmdBuffers.data());
	}

	void Renderer::CreateCommandBuffers()
	{

	}

	void Renderer::BuildCommandBuffers()
	{
		VkCommandBufferBeginInfo commandBufferBeginInfo = VkFactory::CommandBufferBeginInfo();
		
		VkClearValue clearValues[2];
		clearValues[0].color = { { 0.0f, 0.0f, 0.2f, 1.0f } };
		clearValues[1].depthStencil = { 1.0f, 0 };
		
		VkRenderPassBeginInfo renderPassBeginInfo = VkFactory::RenderPassBeginInfo();
		renderPassBeginInfo.renderPass = m_renderPass;
		renderPassBeginInfo.renderArea.offset.x = 0;
		renderPassBeginInfo.renderArea.offset.y = 0;
		renderPassBeginInfo.renderArea.extent.width = m_width;
		renderPassBeginInfo.renderArea.extent.height = m_height;
		/** TODO */
		renderPassBeginInfo.clearValueCount = 2;
		renderPassBeginInfo.pClearValues = clearValues;
		
		for (Int32 i = 0; i < m_drawCmdBuffers.size(); ++i)
		{
			renderPassBeginInfo.framebuffer = m_framebuffers[i];

			VK_CHECK_RESULT(vkBeginCommandBuffer(m_drawCmdBuffers[i], &commandBufferBeginInfo));

			vkCmdBeginRenderPass(m_drawCmdBuffers[i], &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

			VkDeviceSize offsets[1] = { 0 };
			vkCmdBindVertexBuffers(m_drawCmdBuffers[i], 0, 1, &m_vertices->GetVertexBuffer().buffer, offsets);
			vkCmdBindIndexBuffer(m_drawCmdBuffers[i], m_vertices->GetIndexBuffer().buffer, 0, VK_INDEX_TYPE_UINT32);
			
			VkRect2D scissor = VkFactory::Scissor(m_width, m_height);
			vkCmdSetScissor(m_drawCmdBuffers[i], 0, 1, &scissor);

			vkCmdBindDescriptorSets(m_drawCmdBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipelineLayout, 0, 1, &m_descriptorSet, 0, nullptr);
			
			for (Int32 j = 0; j < m_viewports.size(); ++j)
			{
				vkCmdSetViewport(m_drawCmdBuffers[i], 0, 1, &m_viewports[i]);

				vkCmdBindPipeline(m_drawCmdBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipelines[i]);

				vkCmdDrawIndexed(m_drawCmdBuffers[i], m_vertices->GetIndexCount(), 1, 0, 0, 1);
			}
			
			vkCmdEndRenderPass(m_drawCmdBuffers[i]);

			VK_CHECK_RESULT(vkEndCommandBuffer(m_drawCmdBuffers[i]));
		}
	}

	void Renderer::WindowResize()
	{
		if (!m_prepared)
		{
			return;
		}
		m_prepared = false;

		vkDeviceWaitIdle(m_device->GetLogicalDevice());

		m_width = m_destWidth;
		m_height = m_destHeight;

		m_swapchain->CreateSwapchain(&m_width, &m_height, m_settings.vsync);

		vkDestroyImageView(m_device->GetLogicalDevice(), m_depthStencil.view, nullptr);
		vkDestroyImage(m_device->GetLogicalDevice(), m_depthStencil.image, nullptr);
		vkFreeMemory(m_device->GetLogicalDevice(), m_depthStencil.mem, nullptr);

		InitDepthStencil();
		for (UInt32 i = 0; i < m_framebuffers.size(); ++i) 
		{
			vkDestroyFramebuffer(m_device->GetLogicalDevice(), m_framebuffers[i], nullptr);
		}
		InitFramebuffers();

		// Command buffers need to be recreated as they may store
		// references to the recreated frame buffer
		DestroyCommandBuffers();
		CreateCommandBuffers();
		BuildCommandBuffers();

		vkDeviceWaitIdle(m_device->GetLogicalDevice());

		if (m_settings.overlay) 
		{
			/*
				UIOverlay->resize(width, height, frameBuffers);
			*/
		}

		m_camera.updateAspectRatio((float)m_width / (float)m_height);

		WindowResized();
		ViewChanged();

		m_prepared = true;
	}

	void Renderer::WindowResized()
	{
		m_uniforms->UpdateVS();
	}

	void Renderer::ViewChanged()
	{
		m_uniforms->UpdateVS();
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

		vkDeviceWaitIdle(m_device->GetLogicalDevice());
	}
	
	void Renderer::SubmitVerticesData(std::vector<Vertex> & verticesData, std::vector<UInt32> & indicesData)
	{
		m_vertices->Submit(verticesData, indicesData);
	}
	
	void Renderer::SetViewports(std::vector<VkViewport> & viewports)
	{
		m_viewports = std::move(viewports);
	}
	
	void Renderer::SetPipelines(std::vector<VkPipeline> & pipelines)
	{
		m_pipelines = std::move(pipelines);
		assert(m_pipelines.size() == m_viewports.size());
	}
	
	void Renderer::RenderFrame()
	{
		if (m_viewUpdated)
		{
			m_viewUpdated = false;
			ViewChanged();
		}

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