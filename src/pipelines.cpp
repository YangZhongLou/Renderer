#include "pipelines.h"
#include "device.h"
#include "defines.h"
#include "vk_factory.h"

namespace Concise
{
	Pipelines::Pipelines(Device * device)
	{
		m_device = device;
		
		VkPipelineCacheCreateInfo pipelineCacheCreateInfo = VkFactory::PipelineCacheCreateInfo();
		VK_CHECK_RESULT(vkCreatePipelineCache(m_device->GetLogicalDevice(), &pipelineCacheCreateInfo, nullptr, &m_pipelineCache))
	}
	
	~Pipelines()
	{
		
	}
}