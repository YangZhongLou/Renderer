#include "synchronizations.h"
#include "vk_factory.hpp"
#include "device.h"
#include "defines.h"

namespace Concise
{
	Synchronizations::Synchronizations()
	{
		VkSemaphoreCreateInfo semaphoreCreateInfo = 
			VkFactory::SemaphoreCreateInfo();
		VK_CHECK_RESULT(vkCreateSemaphore(LogicalDevice,
			&semaphoreCreateInfo, nullptr, &m_presentCompleteSemaphore));
		VK_CHECK_RESULT(vkCreateSemaphore(LogicalDevice,
			&semaphoreCreateInfo, nullptr, &m_renderCompleteSemaphore));

		VkFenceCreateInfo fenceCreateInfo = 
			VkFactory::FenceCreateInfo(VK_FENCE_CREATE_SIGNALED_BIT);
		VK_CHECK_RESULT(vkCreateFence(LogicalDevice,
			&fenceCreateInfo, nullptr, &m_queueFence));
	}
	
	Synchronizations::~Synchronizations()
	{
		vkDestroySemaphore(LogicalDevice, m_presentCompleteSemaphore, nullptr);
		vkDestroySemaphore(LogicalDevice, m_renderCompleteSemaphore, nullptr);
		vkDestroyFence(LogicalDevice, m_queueFence, nullptr);
	}
}