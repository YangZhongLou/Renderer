#include "descriptors.h"
#include "device.h"

namespace Concise
{
	Descriptors::Descriptors(Device * device)
	{
		m_device = device;
		/** TODO, refactor this later */
		VkDescriptorPoolSize typeCounts[1];
		typeCounts[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		typeCounts[0].descriptorCount = 1;

		VkDescriptorPoolCreateInfo descriptorPoolInfo = VkFactory::DescriptorPoolCreateInfo();
		descriptorPoolInfo.poolSizeCount = 1;
		descriptorPoolInfo.pPoolSizes = typeCounts;
		descriptorPoolInfo.maxSets = 1;

		VK_CHECK_RESULT(vkCreateDescriptorPool(m_device->GetLogicalDevice(), &descriptorPoolInfo, nullptr, &m_descriptorPool));
		
				
		VkDescriptorSetLayoutBinding layoutBinding = VkFactory::DescriptorSetLayoutBinding(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT);
		VkDescriptorSetLayoutCreateInfo descriptorLayoutCreateInfo = VkFactory::DescriptorSetLayoutCreateInfo(layoutBinding);

		VK_CHECK_RESULT(vkCreateDescriptorSetLayout(device->GetLogicalDevice(), &descriptorLayoutCreateInfo, nullptr, &m_descriptorSetLayout));
	}
	
	Descriptors::~Descriptors()
	{
		
	}
}