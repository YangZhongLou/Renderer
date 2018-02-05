 /**
* Author ZhongLou Yang, 2018-2-5
*/

#pragma once

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
	};
}