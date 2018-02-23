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

		VK_CHECK_RESULT(vkCreateDescriptorSetLayout(m_device->GetLogicalDevice(), &descriptorLayoutCreateInfo, nullptr, &m_descriptorSetLayout));
		
		VkDescriptorSetAllocateInfo descriptorSetAllocateInfo = VkFactory::DescriptorSetAllocateInfo(m_descriptorSetLayout, m_descriptorPool);
		VK_CHECK_RESULT(vkAllocateDescriptorSets(m_device->GetLogicalDevice(), &descriptorSetAllocateInfo, &m_descriptorSet));

		VkWriteDescriptorSet writeDescriptorSet = VkFactory::WriteDescriptorSet(m_descriptorSet, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, m_uniforms->GetDescriptorBufferInfo());
		vkUpdateDescriptorSets(m_device->GetLogicalDevice(), 1, &writeDescriptorSet, 0, nullptr);
	}
	
	Descriptors::~Descriptors()
	{
		
	}
}