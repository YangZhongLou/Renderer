 /**
* Author ZhongLou Yang, 2018-2-5
*/

#pragma once

#include <vector>
#include "types.h"

namespace Concise
{
	class Synchronizations
	{
	private:
		VkSemaphore m_presentCompleteSemaphore;
		VkSemaphore m_renderCompleteSemaphore;
		VkFence m_queueFence;
	public:
		Synchronizations();
		~Synchronizations();
	public:
		VkSemaphore GetPresentCompleteSemaphore() const { return m_presentCompleteSemaphore; }
		VkSemaphore GetRenderCompleteSemaphore() const { return m_renderCompleteSemaphore; }
		VkFence GetQueueFence() const { return m_queueFence; }
	};
}