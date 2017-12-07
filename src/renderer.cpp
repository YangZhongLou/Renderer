#include "renderer.h"
#include "device.h"
#include "vkfactory.hpp"
#include "utils.h"

namespace Concise
{
	Renderer::Renderer()
	{
	}
	
	Renderer::~Renderer()
	{
		/** TODO, RAII */
		vkDestroySemaphore(m_device->GetLogicalDevice(), presentCompleteSemaphore, nullptr);
		vkDestroySemaphore(m_device->GetLogicalDevice(), renderCompleteSemaphore, nullptr);

		for (auto& fence : m_fences)
		{
			vkDestroyFence(m_device->GetLogicalDevice(), fence, nullptr);
		}
	}
	
	void Renderer::Init()
	{
	}
	
	void Renderer::InitSync()
	{
		VkSemaphoreCreateInfo semaphoreCreateInfo = VkFactory::SemaphoreCreateInfo();
		Utils::VK_CHECK_RESULT(vkCreateSemaphore(m_device->GetLogicalDevice(), &semaphoreCreateInfo, nullptr, &m_presentSemaphore));
		Utils::VK_CHECK_RESULT(vkCreateSemaphore(m_device->GetLogicalDevice(), &semaphoreCreateInfo, nullptr, &m_renderSemaphore));
		
		VkFenceCreateInfo fenceCreateInfo = VkFactory::FenceCreateInfo(VK_FENCE_CREATE_SIGNALED_BIT);
		m_fences.resize(m_drawCmdBuffers.size());
		for (auto& fence : m_fences)
		{
			Utils::VK_CHECK_RESULT(vkCreateFence(m_device->GetLogicalDevice(), &fenceCreateInfo, nullptr, &fence));
		}
	}
	
	void Device::BuildCommandBuffers()
	{
		VkCommandBufferBeginInfo commandBufferBeginInfo = VkFactory::CommandBufferBeginInfo();
		
		VkClearValue clearValues[2];
		clearValues[0].color = { { 0.0f, 0.0f, 0.2f, 1.0f } };
		clearValues[1].depthStencil = { 1.0f, 0 };
		
		VkRenderPassBeginInfo renderPassBeginInfo = VkFactory::RenderPassBeginInfo();
		renderPassBeginInfo.renderPass = renderPass;
		renderPassBeginInfo.renderArea.offset.x = 0;
		renderPassBeginInfo.renderArea.offset.y = 0;
		renderPassBeginInfo.renderArea.extent.width = width;
		renderPassBeginInfo.renderArea.extent.height = height;
		/** TODO */
		renderPassBeginInfo.clearValueCount = 2;
		renderPassBeginInfo.pClearValues = clearValues;
		
		for (int32_t i = 0; i < m_drawCommandBuffers.size(); ++i)
		{
			renderPassBeginInfo.framebuffer = m_frameBuffers[i];

			Utils::VK_CHECK_RESULT(vkBeginCommandBuffer(m_drawCommandBuffers[i], &cmdBufInfo));

			vkCmdBeginRenderPass(m_drawCommandBuffers[i], &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

			VkViewport viewport = VkFactory::ViewPort(static_cast<float>m_width, static_cast<float>m_height, 0.0f, 1.0f);
			vkCmdSetViewport(m_drawCommandBuffers[i], 0, 1, &viewport);

			VkRect2D scissor = VkFactory::Scissor();
			vkCmdSetScissor(m_drawCommandBuffers[i], 0, 1, &scissor);

			vkCmdBindDescriptorSets(m_drawCommandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1, &descriptorSet, 0, nullptr);

			vkCmdBindPipeline(m_drawCommandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);

			VkDeviceSize offsets[1] = { 0 };
			vkCmdBindVertexBuffers(m_drawCommandBuffers[i], 0, 1, &vertices.buffer, offsets);

			vkCmdBindIndexBuffer(m_drawCommandBuffers[i], indices.buffer, 0, VK_INDEX_TYPE_UINT32);

			vkCmdDrawIndexed(m_drawCommandBuffers[i], indices.count, 1, 0, 0, 1);

			vkCmdEndRenderPass(m_drawCommandBuffers[i]);

			Utils::VK_CHECK_RESULT(vkEndCommandBuffer(m_drawCommandBuffers[i]));
		}
	}
}