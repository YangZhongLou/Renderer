#include "synchronizations.h"
#include "vk_factory.hpp"
#include "device.h"
#include "defines.h"

namespace Concise
{
	Synchronizations::Synchronizations(UInt32 fenceSize)
	{
		VkSemaphoreCreateInfo semaphoreCreateInfo = VkFactory::SemaphoreCreateInfo();
		VK_CHECK_RESULT(vkCreateSemaphore(Device::Instance().GetLogicalDevice(),
			&semaphoreCreateInfo, nullptr, &m_presentCompleteSemaphore));
		VK_CHECK_RESULT(vkCreateSemaphore(Device::Instance().GetLogicalDevice(),
			&semaphoreCreateInfo, nullptr, &m_renderCompleteSemaphore));

		VkFenceCreateInfo fenceCreateInfo = VkFactory::FenceCreateInfo(VK_FENCE_CREATE_SIGNALED_BIT);
		m_fences.resize(fenceSize);
		for (auto & fence : m_fences)
		{
			VK_CHECK_RESULT(vkCreateFence(Device::Instance().GetLogicalDevice(),
				&fenceCreateInfo, nullptr, &fence));
		}
	}
	
	Synchronizations::~Synchronizations()
	{
		
	}
}