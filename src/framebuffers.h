 /**
* Author ZhongLou Yang, 2018-2-5
*/

#pragma once

#include <vector>
#include <vulkan/vulkan.h>
#include "types.h"

namespace Concise
{
	class Framebuffers
	{
	private:
		std::vector<VkFramebuffer> m_frameBuffers;
	public:
		Framebuffers(UInt32 size);
		~Framebuffers();
	};
}