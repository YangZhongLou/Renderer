#include "descriptors.h"
#include "device.h"
#include "vk_factory.hpp"
#include "defines.h"

namespace Concise
{
	Descriptors::Descriptors()
	{
		std::vector<VkDescriptorPoolSize> poolSizes = 
		{
			VkFactory::DescriptorPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, UNIFORM_BUFFER_COUNT),
			VkFactory::DescriptorPoolSize(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, IMAGE_SAMPLER_COUNT)
		};
		VkDescriptorPoolCreateInfo descriptorPoolInfo = 
			VkFactory::DescriptorPoolCreateInfo(2, poolSizes.size(), poolSizes.data());
		VK_CHECK_RESULT(vkCreateDescriptorPool(LogicalDevice,
			&descriptorPoolInfo, nullptr, &m_descriptorPool));

		setLayoutBindings.resize(UNIFORM_BUFFER_COUNT + IMAGE_SAMPLER_COUNT);
		for (auto i = 0; i < UNIFORM_BUFFER_COUNT; ++i)
		{
			setLayoutBindings[i] = VkFactory::DescriptorSetLayoutBinding(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, i);
		}

		for (auto i = UNIFORM_BUFFER_COUNT; i < setLayoutBindings.size(); ++i)
		{
			setLayoutBindings[i] = VkFactory::DescriptorSetLayoutBinding(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT, i);
		}

		VkDescriptorSetLayoutCreateInfo descriptorLayout = VkFactory::DescriptorSetLayoutCreateInfo(
			setLayoutBindings.size(),
			setLayoutBindings.data()
		);
		VK_CHECK_RESULT(vkCreateDescriptorSetLayout(LogicalDevice, 
			&descriptorLayout, nullptr, &m_descriptorSetLayout));
	}
	
	Descriptors::~Descriptors()
	{
		
	}
}