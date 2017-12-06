/*
* This code is licensed under the MIT license (MIT) (http://opensource.org/licenses/MIT)
*/
#pragma once

#include <vulkan/vulkan.h>
#include "types.h"

namespace Concise
{
	class Device
	{
	private:
		VkPhysicalDevice m_physicalDevice;
		VkPhysicalDeviceProperties m_deviceProperties;
		VkPhysicalDeviceFeatures m_deviceFeatures;
		VkPhysicalDeviceMemoryProperties m_deviceMemoryProperties;
		VkPhysicalDeviceFeatures m_enabledFeatures {};
		VkDevice m_device;
		
		VkQueue m_queue;
		VkCommandPool m_cmdPool;
		
		VkSemaphoreCreateInfo m_presentSemaphore;
		VkSemaphoreCreateInfo m_renderSemaphore;
		
		std::vector<VkFence> m_fences;
	public:
		Device();
		~Device();
	public:
		void Init();
		VkDevice GetLogicalDevice() const { return m_device; }
		VkPhysicalDevice GetPhysicalDevice() const { return m_physicalDevice; }
		UInt32 GetMemoryTypeIndex(UInt32 typeBits, VkMemoryPropertyFlags properties);
		
		VkCommandBuffer GetCommandBuffer(bool beginRecord);
		void FlushCommandBuffer(VkCommandBuffer commandBuffer);
	}
}