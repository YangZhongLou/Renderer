/**
* Author ZhongLou Yang, 2018-2-6
*/

#pragma once

#include <vulkan/vulkan.h>

namespace Concise
{
	class Device;
	class Descriptor
	{
	private:
		Device * m_device;
		VkDescriptorSetLayout m_descriptorSetLayout;
		VkDescriptorSet m_descriptorSet;
	public:
		Descriptor(Device * device);
		~Descriptor();
	};
}
