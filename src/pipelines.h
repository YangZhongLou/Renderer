 /**
* Author ZhongLou Yang, 2018-2-5
*/

#pragma once

#include <map>
#include <string>
#include <vulkan/vulkan.h>

namespace Concise
{
	class Device;
	
	struct Pipeline
	{
		VkPipelineLayout m_pipelineLayout;
		VkPipeline m_pipeline;
	};
	
	class Pipelines
	{
	private:
		Device * m_device;
		VkPipelineCache m_pipelineCache;
		std::map<std::string, Pipeline&> m_pipelines;
	public:
		Pipelines(Device * device);
		~Pipelines();
	};
}