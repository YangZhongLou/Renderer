/**
* Author ZhongLou Yang, 2018-2-5
*/
#pragma once

#include <vector>
#include <vulkan/vulkan.h>
#include "types.h"

namespace Concise
{
	struct Buffer;
	class VulkanInstance;
	
	class Device
	{
	private:
		VkPhysicalDevice m_physicalDevice;
		VkPhysicalDeviceProperties m_deviceProperties;
		VkPhysicalDeviceFeatures m_deviceFeatures;
		VkPhysicalDeviceMemoryProperties m_deviceMemoryProperties;
		std::vector<VkQueueFamilyProperties> m_queueFamilyProperties;
		std::vector<std::string> m_supportedExtensions;

		VkPhysicalDeviceFeatures m_enabledFeatures {};
		std::vector<const char*> m_enabledExtensions;
		VkDevice m_logicalDevice;
		
		struct
		{
			UInt32 graphics;
			UInt32 compute;
			UInt32 transfer;
		} m_queueFamilyIndices;
		
		VkQueue m_queue;
		VkCommandPool m_cmdPool;
		
		VkSemaphoreCreateInfo m_presentSemaphore;
		VkSemaphoreCreateInfo m_renderSemaphore;
		
		VkFormat m_depthFormat;
		
		std::vector<VkFence> m_fences;

		struct
		{
			VkImage image;
			VkDeviceMemory mem;
			VkImageView view;
		} m_depthStencil;
		
		VulkanInstance * m_vkInstance;
	public:
		static Device & Instance();
	private:
		Device();
		~Device();
	public:
		VulkanInstance * GetVulkanInstance() const { return m_vkInstance; }
		VkDevice GetLogicalDevice() const { return m_logicalDevice; }
		VkPhysicalDevice GetPhysicalDevice() const { return m_physicalDevice; }
		UInt32 GetMemoryTypeIndex(UInt32 typeBits, VkMemoryPropertyFlags properties);
		VkFormat GetSupportedDepthFormat() const { return m_depthFormat; }
		VkCommandPool GetCommandPool() const { return m_cmdPool; }
		VkQueue GetQueue() const { return m_queue; }
		UInt32 GetQueueFamilyIndex(VkQueueFlagBits queueFlags);
		
		bool ExtensionSupported(std::string extension);
		
		VkCommandBuffer GetCommandBuffer(bool beginRecord);
		void FlushCommandBuffer(VkCommandBuffer commandBuffer);
		
		/** TODO */
		void EnableFeatures();
		
		VkResult CreateBuffer(VkBufferUsageFlags usageFlags, VkMemoryPropertyFlags memoryPropertyFlags, VkDeviceSize size, Buffer & buffer, void *data = nullptr);
		void CreateCommandPool(UInt32 queueFamilyIndex, VkCommandPoolCreateFlags createFlags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);
		void CreateLogicalDevice(VkPhysicalDeviceFeatures & enabledFeatures, 
			std::vector<const char*> enabledExtensions, 
			bool useSwapChain = true, 
			VkQueueFlags requestedQueueTypes = VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_COMPUTE_BIT);
	private:
		void InitDepthStencil(UInt32 width, UInt32 height);
	};
}