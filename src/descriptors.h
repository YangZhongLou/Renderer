/**
* Author ZhongLou Yang, 2018-2-5
*/

#pragma once

#include <vector>
#include <vulkan/vulkan.h>

namespace Concise
{
	class Device;
	
	class Descriptors
	{
	private:
		Device * m_device;
		VkDescriptorPool m_descriptorPool = VK_NULL_HANDLE;
		std::vector<VkDescriptorSetLayout> m_layouts;
		std::vector<VkDescriptorSet> m_sets;
	public:
		Descriptors();
		~Descriptors();
	public:
		void Add(Descriptor * descriptor) { m_descriptors.push_back(descriptor); }
	};
}