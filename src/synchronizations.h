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
		std::vector<VkFence> m_fences;
	public:
		Synchronizations(UInt32 fenceSize);
		~Synchronizations();
	public:
		VkSemaphore GetPresentCompleteSemaphore() const { return m_presentCompleteSemaphore; }
		VkSemaphore GetRenderCompleteSemaphore() const { return m_renderCompleteSemaphore; }
		VkFence GetFence(UInt32 i) const { return m_fences[i]; }
	};
}