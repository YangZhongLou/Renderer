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

namespace Concise
{
	Renderer::Renderer()
	{
		m_swapchain = nullptr;
		
		
		
		m_uniforms = new Uniforms(m_device, this);
	}
	
	Renderer::~Renderer()
	{
		DestroyCommandBuffers();

		if (m_descriptorPool != VK_NULL_HANDLE)
		{
			vkDestroyDescriptorPool(m_device->GetLogicalDevice(), m_descriptorPool, nullptr);
		}

		vkDestroyRenderPass(m_device->GetLogicalDevice(), m_renderPass, nullptr);

		for (UInt32 i = 0; i < m_framebuffers.size(); i++)
		{
			vkDestroyFramebuffer(m_device->GetLogicalDevice(), m_framebuffers[i], nullptr);
		}

		/** TODO, RAII */
		vkDestroySemaphore(m_device->GetLogicalDevice(), m_presentCompleteSemaphore, nullptr);
		vkDestroySemaphore(m_device->GetLogicalDevice(), m_renderCompleteSemaphore, nullptr);

		for (auto& fence : m_fences)
		{
			vkDestroyFence(m_device->GetLogicalDevice(), fence, nullptr);
		}
		
		vkDestroyPipeline(m_device->GetLogicalDevice(), m_pipeline, nullptr);

		vkDestroyPipelineLayout(m_device->GetLogicalDevice(), m_pipelineLayout, nullptr);
		vkDestroyDescriptorSetLayout(m_device->GetLogicalDevice(), m_descriptorSetLayout, nullptr);
		vkDestroyPipelineCache(m_device->GetLogicalDevice(), m_pipelineCache, nullptr);

		vkDestroyImageView(m_device->GetLogicalDevice(), m_depthStencil.view, nullptr);
		vkDestroyImage(m_device->GetLogicalDevice(), m_depthStencil.image, nullptr);
		vkFreeMemory(m_device->GetLogicalDevice(), m_depthStencil.mem, nullptr);

		SAFE_DELETE(m_uniforms);
		SAFE_DELETE(m_vertices);
		SAFE_DELETE(m_swapchain);
		SAFE_DELETE(m_debugger);
		SAFE_DELETE(m_device);

		vkDestroyInstance(m_vkInstance, nullptr);
	}
	
	void Renderer::Init()
	{
		m_prepared = false;
		InitUniforms();
		InitVeritces();
		InitDepthStencil();
		InitRenderPass();
		InitDescriptorSetLayout();
		InitDescriptorPool();
		InitDescriptorSet();
		InitPipelineCache();
		InitFramebuffers();
		InitPipelines();
		m_prepared = true;
	}
	
	void Renderer::InitVeritces()
	{
		m_vertices = new Vertices(m_device);
	}
	
	void Renderer::DestroyCommandBuffers()
	{
		vkFreeCommandBuffers(m_device->GetLogicalDevice(), m_device->GetCommandPool(), static_cast<UInt32>(m_drawCmdBuffers.size()), m_drawCmdBuffers.data());
	}

	void Renderer::CreateCommandBuffers()
	{
		m_drawCmdBuffers.resize(m_swapchain->GetImageCount());

		VkCommandBufferAllocateInfo cmdBufAllocateInfo = VkFactory::CommandBufferAllocateInfo(m_device->GetCommandPool(), VK_COMMAND_BUFFER_LEVEL_PRIMARY, static_cast<UInt32>(m_drawCmdBuffers.size()));

		VK_CHECK_RESULT(vkAllocateCommandBuffers(m_device->GetLogicalDevice(), &cmdBufAllocateInfo, m_drawCmdBuffers.data()));
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
	
	void Renderer::InitVulkanDebugger()
	{
		if (!m_settings.validation)
		{
			return;
		}

		VkDebugReportFlagsEXT debugReportFlags = VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT;
		m_debugger = new Debugger(m_vkInstance, debugReportFlags);
		m_debugger->Init();
	}

	void Renderer::InitVulkanDevice()
	{
		m_device = new Device(m_vkInstance);
		m_device->Init();
	}
	
	void Renderer::InitSwapchain()
	{
		m_swapchain = new Swapchain(m_vkInstance, m_device, this);
		m_swapchain->Init();
		m_swapchain->CreateSwapchain(&m_width, &m_height, m_settings.vsync);
	}

	void Renderer::InitCommandBuffers()
	{
		CreateCommandBuffers();
	}

	void Renderer::InitUniforms()
	{

	}
	
	void Renderer::InitDescriptorSet()
	{

	}
	
	void Renderer::InitDepthStencil()
	{
		VkImageCreateInfo image = VkFactory::ImageCreateInfo(m_device->GetSupportedDepthFormat(), {m_width, m_height, 1});
		VK_CHECK_RESULT(vkCreateImage(m_device->GetLogicalDevice(), &image, nullptr, &m_depthStencil.image));

		VkMemoryRequirements memReqs;
		vkGetImageMemoryRequirements(m_device->GetLogicalDevice(), m_depthStencil.image, &memReqs);
		VkMemoryAllocateInfo memAlloc = VkFactory::MemoryAllocateInfo(memReqs.size, m_device->GetMemoryTypeIndex(memReqs.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT));

		VK_CHECK_RESULT(vkAllocateMemory(m_device->GetLogicalDevice(), &memAlloc, nullptr, &m_depthStencil.mem));
		VK_CHECK_RESULT(vkBindImageMemory(m_device->GetLogicalDevice(), m_depthStencil.image, m_depthStencil.mem, 0));

		VkImageSubresourceRange imageSubresourceRange = VkFactory::ImageSubresourceRange(VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT);
		VkImageViewCreateInfo imageViewCreateInfo = VkFactory::ImageViewCreateInfo(m_device->GetSupportedDepthFormat(), m_depthStencil.image, imageSubresourceRange);
		VK_CHECK_RESULT(vkCreateImageView(m_device->GetLogicalDevice(), &imageViewCreateInfo, nullptr, &m_depthStencil.view));
	}
	
	void Renderer::InitFramebuffers()
	{
		m_framebuffers.resize(m_swapchain->GetImageCount());
		for (UInt32 i = 0; i < static_cast<UInt32>(m_framebuffers.size()); i++)
		{
			std::vector<VkImageView> attachments(2);										
			attachments[0] = m_swapchain->GetBuffer(i).view;
			attachments[1] = m_depthStencil.view;									

			VkFramebufferCreateInfo frameBufferCreateInfo = VkFactory::FramebufferCreateInfo(m_renderPass, attachments, m_width, m_height);
			VK_CHECK_RESULT(vkCreateFramebuffer(m_device->GetLogicalDevice(), &frameBufferCreateInfo, nullptr, &m_framebuffers[i]));
		}
	}
	
	void Renderer::InitRenderPass()
	{
		std::vector<VkAttachmentDescription> attachments;
		// Color attachment
		attachments.push_back(VkFactory::AttachmentDescription(m_swapchain->GetColorFormat()));
		attachments[0].storeOp = VK_ATTACHMENT_STORE_OP_STORE;							
		attachments[0].finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;					
																						
		// Depth attachment
		attachments.push_back(VkFactory::AttachmentDescription(m_device->GetSupportedDepthFormat()));
		attachments[1].finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;	

		// Setup attachment references
		VkAttachmentReference colorReference = VkFactory::AttachmentReference(0, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);			
		VkAttachmentReference depthReference = VkFactory::AttachmentReference(1, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);	

		VkSubpassDescription subpassDescription = VkFactory::SubpassDescription(VK_PIPELINE_BIND_POINT_GRAPHICS);
		subpassDescription.pColorAttachments = &colorReference;							
		subpassDescription.pDepthStencilAttachment = &depthReference;					
		
		std::vector<VkSubpassDependency> dependencies;
		VkFactory::SubpassDependencies(dependencies);

		VkRenderPassCreateInfo renderPassInfo = VkFactory::RenderPassCreateInfo(attachments, dependencies, &subpassDescription);
		VK_CHECK_RESULT(vkCreateRenderPass(m_device->GetLogicalDevice(), &renderPassInfo, nullptr, &m_renderPass));
	}
	
	void Renderer::InitPipelineCache()
	{

	}
	
	void Renderer::InitPipelines()
	{

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