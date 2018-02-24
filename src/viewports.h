/**
* Author ZhongLou Yang, 2018-2-24
*/

#pragma once

#include <vector>
#include <vulkan/vulkan.h>

namespace Concise
{
	class Viewports
	{
	private:
		std::vector<VkViewport> m_viewports;
	public:
		Viewports();
		~Viewports();
	};
}
