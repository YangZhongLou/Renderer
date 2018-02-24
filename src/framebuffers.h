 /**
* Author ZhongLou Yang, 2018-2-5
*/

#pragma once

#include <vector>
#include <vulkan/vulkan.h>
#include "types.h"

namespace Concise
{
	class Swapchain;

	class Framebuffers
	{
	private:
		std::vector<VkFramebuffer> m_framebuffers;
	public:
		Framebuffers(Swapchain * swapchain, VkRenderPass renderpass, 
			UInt32 width, UInt32 height);
		~Framebuffers();
	};
}