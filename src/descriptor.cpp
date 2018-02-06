#include "descriptor.h"
#include "device.h"

namespace Concise
{
	Descriptor::Descriptor(Device * device)
	{
		m_device = device;

		VK_CHECK_RESULT(vkCreatePipelineLayout(device->GetLogicalDevice(), &pipelineLayoutCreateInfo, nullptr, &m_pipelineLayout));
		
	}
	
	Descriptor::~Descriptor()
	{
		
	}
}