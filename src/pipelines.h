 /**
* Author ZhongLou Yang, 2018-2-5
*/

#pragma once

#include <map>
#include <string>
#include <vulkan/vulkan.h>

namespace Concise
{


	class Pipelines
	{
	private:
		VkPipelineCache m_pipelineCache;
		VkPipelineLayout m_pipelineLayout;

		std::map<std::string, VkPipeline> m_pipelines;
	public:
		Pipelines();
		~Pipelines();
	};
}