/**
* Author ZhongLou Yang, 2018-2-5
*/

#pragma once

#include <vulkan/vulkan.h>

namespace Concise
{
	class VulkanInstance
	{
	private:
		VkInstance m_instance;

	public:
		VulkanInstance();
		~VulkanInstance();
	public:
		VkInstance & Get() { return m_instance; }
	};
}