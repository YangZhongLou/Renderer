/*
* This code is licensed under the MIT license (MIT) (http://opensource.org/licenses/MIT)
*/

#include "device.h"
#include "utils.h"
#include "vkfactory.hpp"
#include <assert.h>
#include <iostream>

namespace Concise
{
	Device::Device(VkInstance instance) : m_vkInstance(instance)
	{
	}
	
	Device::~Device()
	{
		if (m_cmdPool)
		{
			vkDestroyCommandPool(m_logicalDevice, m_cmdPool, nullptr);
		}
		
		if (m_logicalDevice)
		{
			vkDestroyDevice(m_logicalDevice, nullptr);
		}
	}
	
	void Device::Init()
	{
		UInt32 gpuCount = 0;
		VK_CHECK_RESULT(vkEnumeratePhysicalDevices(m_vkInstance, &gpuCount, nullptr));
		assert(gpuCount > 0);
		
		std::vector<VkPhysicalDevice> physicalDevices(gpuCount);
		VK_CHECK_RESULT(vkEnumeratePhysicalDevices(m_vkInstance, &gpuCount, physicalDevices.data()));

		UInt32 selectedDevice = 0;
		m_physicalDevice = physicalDevices[selectedDevice];

		vkGetPhysicalDeviceProperties(m_physicalDevice, &m_deviceProperties);
		vkGetPhysicalDeviceFeatures(m_physicalDevice, &m_deviceFeatures);
		vkGetPhysicalDeviceMemoryProperties(m_physicalDevice, &m_deviceMemoryProperties);
		
		UInt32 queueFamilyCount;
		vkGetPhysicalDeviceQueueFamilyProperties(m_physicalDevice, &queueFamilyCount, nullptr);
		assert(queueFamilyCount > 0);
		m_queueFamilyProperties.resize(queueFamilyCount);
		vkGetPhysicalDeviceQueueFamilyProperties(m_physicalDevice, &queueFamilyCount, m_queueFamilyProperties.data());

		UInt32 extCount = 0;
		vkEnumerateDeviceExtensionProperties(m_physicalDevice, nullptr, &extCount, nullptr);
		if (extCount > 0)
		{
			std::vector<VkExtensionProperties> extensions(extCount);
			if (vkEnumerateDeviceExtensionProperties(m_physicalDevice, nullptr, &extCount, &extensions.front()) == VK_SUCCESS)
			{
				for (auto ext : extensions)
				{
					m_supportedExtensions.push_back(ext.extensionName);
				}
			}
		}

		CreateLogicalDevice(m_enabledFeatures, m_enabledExtensions);

		vkGetDeviceQueue(m_logicalDevice, m_queueFamilyIndices.graphics, 0, &m_queue);

		InitSupportedDepthFormat();
	}
	
	void Device::InitSupportedDepthFormat()
	{
		std::vector<VkFormat> depthFormats = {
			VK_FORMAT_D32_SFLOAT_S8_UINT,
			VK_FORMAT_D32_SFLOAT,
			VK_FORMAT_D24_UNORM_S8_UINT,
			VK_FORMAT_D16_UNORM_S8_UINT,
			VK_FORMAT_D16_UNORM
		};

		for (auto& format : depthFormats)
		{
			VkFormatProperties formatProps;
			vkGetPhysicalDeviceFormatProperties(m_physicalDevice, format, &formatProps);
			if (formatProps.optimalTilingFeatures & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT)
			{
				m_depthFormat = format;
				return;
			}
		}
		
		assert(false);
	}
	
	UInt32 Device::GetQueueFamilyIndex(VkQueueFlagBits queueFlags)
	{
		if (queueFlags & VK_QUEUE_COMPUTE_BIT)
		{
			for (UInt32 i = 0; i < static_cast<UInt32>(m_queueFamilyProperties.size()); i++)
			{
				if ((m_queueFamilyProperties[i].queueFlags & queueFlags) && ((m_queueFamilyProperties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) == 0))
				{
					return i;
					break;
				}
			}
		}

		if (queueFlags & VK_QUEUE_TRANSFER_BIT)
		{
			for (UInt32 i = 0; i < static_cast<UInt32>(m_queueFamilyProperties.size()); i++)
			{
				if ((m_queueFamilyProperties[i].queueFlags & queueFlags) && ((m_queueFamilyProperties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) == 0) && ((m_queueFamilyProperties[i].queueFlags & VK_QUEUE_COMPUTE_BIT) == 0))
				{
					return i;
					break;
				}
			}
		}

		for (UInt32 i = 0; i < static_cast<UInt32>(m_queueFamilyProperties.size()); i++)
		{
			if (m_queueFamilyProperties[i].queueFlags & queueFlags)
			{
				return i;
				break;
			}
		}
		
		assert(false);
		return 0;
	}
	
	void Device::CreateLogicalDevice(VkPhysicalDeviceFeatures & enabledFeatures, 
			std::vector<const char*> enabledExtensions, 
			bool useSwapChain, 
			VkQueueFlags requestedQueueTypes)
	{
		std::vector<VkDeviceQueueCreateInfo> queueCreateInfos{};

		const float defaultQueuePriority(0.0f);

		if (requestedQueueTypes & VK_QUEUE_GRAPHICS_BIT)
		{
			m_queueFamilyIndices.graphics = GetQueueFamilyIndex(VK_QUEUE_GRAPHICS_BIT);
			VkDeviceQueueCreateInfo queueInfo = VkFactory::DeviceQueueCreateInfo(m_queueFamilyIndices.graphics, &defaultQueuePriority);
			queueCreateInfos.push_back(queueInfo);
		}
		else
		{
			m_queueFamilyIndices.graphics = VK_NULL_HANDLE;
		}

		if (requestedQueueTypes & VK_QUEUE_COMPUTE_BIT)
		{
			m_queueFamilyIndices.compute = GetQueueFamilyIndex(VK_QUEUE_COMPUTE_BIT);
			if (m_queueFamilyIndices.compute != m_queueFamilyIndices.graphics)
			{
				VkDeviceQueueCreateInfo queueInfo = VkFactory::DeviceQueueCreateInfo(m_queueFamilyIndices.compute, &defaultQueuePriority);;
				queueCreateInfos.push_back(queueInfo);
			}
		}
		else
		{
			m_queueFamilyIndices.compute = m_queueFamilyIndices.graphics;
		}

		if (requestedQueueTypes & VK_QUEUE_TRANSFER_BIT)
		{
			m_queueFamilyIndices.transfer = GetQueueFamilyIndex(VK_QUEUE_TRANSFER_BIT);
			if ((m_queueFamilyIndices.transfer != m_queueFamilyIndices.graphics) && (m_queueFamilyIndices.transfer != m_queueFamilyIndices.compute))
			{
				VkDeviceQueueCreateInfo queueInfo = VkFactory::DeviceQueueCreateInfo(m_queueFamilyIndices.transfer, &defaultQueuePriority);
				queueCreateInfos.push_back(queueInfo);
			}
		}
		else
		{
			m_queueFamilyIndices.transfer = m_queueFamilyIndices.graphics;
		}

		std::vector<const char*> deviceExtensions(enabledExtensions);
		if (useSwapChain)
		{
			deviceExtensions.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
		}

		VkDeviceCreateInfo deviceCreateInfo = VkFactory::DeviceCreateInfo(queueCreateInfos, enabledFeatures);

		if (ExtensionSupported(VK_EXT_DEBUG_MARKER_EXTENSION_NAME))
		{
			deviceExtensions.push_back(VK_EXT_DEBUG_MARKER_EXTENSION_NAME);
		}

		if (deviceExtensions.size() > 0)
		{
			deviceCreateInfo.enabledExtensionCount = static_cast<UInt32>(deviceExtensions.size());
			deviceCreateInfo.ppEnabledExtensionNames = deviceExtensions.data();
		}

		VkResult result = vkCreateDevice(m_physicalDevice, &deviceCreateInfo, nullptr, &m_logicalDevice);

		if (result == VK_SUCCESS)
		{
			CreateCommandPool(m_queueFamilyIndices.graphics);
		}
	}
	
	void Device::CreateCommandPool(UInt32 queueFamilyIndex, VkCommandPoolCreateFlags createFlags)
	{
		VkCommandPoolCreateInfo cmdPoolInfo = VkFactory::CommandPoolCreateInfo(queueFamilyIndex, createFlags);
		VK_CHECK_RESULT(vkCreateCommandPool(m_logicalDevice, &cmdPoolInfo, nullptr, &m_cmdPool));
	}
	
	bool Device::ExtensionSupported(std::string extension)
	{
		return (std::find(m_supportedExtensions.begin(), m_supportedExtensions.end(), extension) != m_supportedExtensions.end());
	}
	
	UInt32 Device::GetMemoryTypeIndex(UInt32 typeBits, VkMemoryPropertyFlags properties)
	{
		for (UInt32 i = 0; i < m_deviceMemoryProperties.memoryTypeCount; i++)
		{
			if ((typeBits & 1) == 1)
			{
				if ((m_deviceMemoryProperties.memoryTypes[i].propertyFlags & properties) == properties)
				{						
					return i;
				}
			}
			typeBits >>= 1;
		}
		
		LOG_ERROR("GetMemoryTypeIndex");
		return 0;
	}
	
	VkCommandBuffer Device::GetCommandBuffer(bool beginRecord)
	{
		VkCommandBuffer cmdBuffer;

		VkCommandBufferAllocateInfo cmdBufAllocateInfo = VkFactory::CommandBufferAllocateInfo(m_cmdPool, VK_COMMAND_BUFFER_LEVEL_PRIMARY);
		VK_CHECK_RESULT(vkAllocateCommandBuffers(m_logicalDevice, &cmdBufAllocateInfo, &cmdBuffer));

		if (beginRecord)
		{
			VkCommandBufferBeginInfo cmdBufInfo = VkFactory::CommandBufferBeginInfo();
			VK_CHECK_RESULT(vkBeginCommandBuffer(cmdBuffer, &cmdBufInfo));
		}

		return cmdBuffer;
	}
	
	void Device::FlushCommandBuffer(VkCommandBuffer commandBuffer)
	{
		assert(commandBuffer != VK_NULL_HANDLE);

		VK_CHECK_RESULT(vkEndCommandBuffer(commandBuffer));

		std::vector<VkCommandBuffer> commandBuffers;
		commandBuffers.push_back(commandBuffer);
		VkSubmitInfo submitInfo = VkFactory::SubmitInfo(commandBuffers);

		VkFenceCreateInfo fenceCreateInfo = VkFactory::FenceCreateInfo();
		VkFence fence;
		VK_CHECK_RESULT(vkCreateFence(m_logicalDevice, &fenceCreateInfo, nullptr, &fence));

		VK_CHECK_RESULT(vkQueueSubmit(m_queue, 1, &submitInfo, fence));
		VK_CHECK_RESULT(vkWaitForFences(m_logicalDevice, 1, &fence, VK_TRUE, DEFAULT_FENCE_TIMEOUT));

		/**TODO, RAII */
		vkDestroyFence(m_logicalDevice, fence, nullptr);
		vkFreeCommandBuffers(m_logicalDevice, m_cmdPool, 1, &commandBuffer);
	}
	
	void Device::EnableFeatures()
	{
		
	}
}