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
}