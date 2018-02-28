/**
* Author ZhongLou Yang, 2018-2-24
*/

#pragma once

#include <vector>
#include <vulkan/vulkan.h>
#include "types.h"

namespace Concise
{
	class CommandBuffers
	{
	private:
		std::vector<VkCommandBuffer> m_drawCmdBuffers;
	public:
		CommandBuffers(UInt32 size);
		~CommandBuffers();
	};
}