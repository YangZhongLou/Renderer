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
	
	struct Descriptor
	{
		VkDescriptorSetLayout layout;
		VkDescriptorSet set;
	};
	
	class Descriptors
	{
	private:
		Device * m_device;
		VkDescriptorPool m_descriptorPool = VK_NULL_HANDLE;
		std::map<std::string, Descriptor&> m_descriptors;
	public:
		Descriptors();
		~Descriptors();
	public:
	};
}