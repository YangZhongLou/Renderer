 /**
* Author ZhongLou Yang, 2018-2-5
*/

#pragma once

#include <vector>
#include <vulkan/vulkan.h>

namespace Concise
{
	class Device;
	class Pipeline;
	
	class Pipelines
	{
	private:
		Device * m_device;
		VkPipelineCache m_pipelineCache;
		std::vector<Pipeline*> m_pipelines;
	public:
		Pipelines(Device * device);
		~Pipelines();
	};
}