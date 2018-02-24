#include "pipelines.h"
#include "device.h"
#include "defines.h"
#include "vk_factory.hpp"

namespace Concise
{
	Pipelines::Pipelines()
	{		
		VkPipelineCacheCreateInfo pipelineCacheCreateInfo = VkFactory::PipelineCacheCreateInfo();
		VK_CHECK_RESULT(vkCreatePipelineCache(Device::Instance().GetLogicalDevice(), 
			&pipelineCacheCreateInfo, nullptr, &m_pipelineCache))

		
	}
	
	Pipelines::~Pipelines()
	{
		
	}
}