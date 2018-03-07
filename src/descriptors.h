/**
* Author ZhongLou Yang, 2018-2-5
*/

#pragma once

#include <map>
#include <string>
#include <vulkan/vulkan.h>

namespace Concise
{
#define UNIFORM_BUFFER_COUNT 4
#define IMAGE_SAMPLER_COUNT 6

	class Descriptors
	{
	private:
		VkDescriptorPool m_descriptorPool = VK_NULL_HANDLE;
		VkDescriptorSetLayout m_descriptorSetLayout;
	public:
		Descriptors();
		~Descriptors();
	public:
	};
}