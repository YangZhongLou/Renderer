/*
* This code is licensed under the MIT license (MIT) (http://opensource.org/licenses/MIT)
*/

#include "device.h"
#include "utils.h"
#include "vkfactory.h"

namespace Concise
{
	Device::Device()
	{
		
	}
	
	Device::~Device()
	{
		
	}
	
	Device::Init()
	{
		if (settings.validation)
		{
			VkDebugReportFlagsEXT debugReportFlags = VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT;
			vks::debug::setupDebugging(instance, debugReportFlags, VK_NULL_HANDLE);
		}

	// Physical device
	uint32_t gpuCount = 0;
	// Get number of available physical devices
	VK_CHECK_RESULT(vkEnumeratePhysicalDevices(instance, &gpuCount, nullptr));
	assert(gpuCount > 0);
	// Enumerate devices
	std::vector<VkPhysicalDevice> physicalDevices(gpuCount);
	err = vkEnumeratePhysicalDevices(instance, &gpuCount, physicalDevices.data());
	if (err) {
		vks::tools::exitFatal("Could not enumerate physical devices : \n" + vks::tools::errorString(err), "Fatal error", !benchmark.active);
	}

	// GPU selection

	// Select physical device to be used for the Vulkan example
	// Defaults to the first device unless specified by command line
	uint32_t selectedDevice = 0;

#if !defined(VK_USE_PLATFORM_ANDROID_KHR)	
	// GPU selection via command line argument
	for (size_t i = 0; i < args.size(); i++)
	{
		// Select GPU
		if ((args[i] == std::string("-g")) || (args[i] == std::string("-gpu")))
		{
			char* endptr;
			uint32_t index = strtol(args[i + 1], &endptr, 10);
			if (endptr != args[i + 1]) 
			{ 
				if (index > gpuCount - 1)
				{
					std::cerr << "Selected device index " << index << " is out of range, reverting to device 0 (use -listgpus to show available Vulkan devices)" << std::endl;
				} 
				else
				{
					std::cout << "Selected Vulkan device " << index << std::endl;
					selectedDevice = index;
				}
			};
			break;
		}
		// List available GPUs
		if (args[i] == std::string("-listgpus"))
		{
			uint32_t gpuCount = 0;
			VK_CHECK_RESULT(vkEnumeratePhysicalDevices(instance, &gpuCount, nullptr));
			if (gpuCount == 0) 
			{
				std::cerr << "No Vulkan devices found!" << std::endl;
			}
			else 
			{
				// Enumerate devices
				std::cout << "Available Vulkan devices" << std::endl;
				std::vector<VkPhysicalDevice> devices(gpuCount);
				VK_CHECK_RESULT(vkEnumeratePhysicalDevices(instance, &gpuCount, devices.data()));
				for (uint32_t i = 0; i < gpuCount; i++) {
					VkPhysicalDeviceProperties deviceProperties;
					vkGetPhysicalDeviceProperties(devices[i], &deviceProperties);
					std::cout << "Device [" << i << "] : " << deviceProperties.deviceName << std::endl;
					std::cout << " Type: " << vks::tools::physicalDeviceTypeString(deviceProperties.deviceType) << std::endl;
					std::cout << " API: " << (deviceProperties.apiVersion >> 22) << "." << ((deviceProperties.apiVersion >> 12) & 0x3ff) << "." << (deviceProperties.apiVersion & 0xfff) << std::endl;
				}
			}
		}
	}
#endif

	physicalDevice = physicalDevices[selectedDevice];

	// Store properties (including limits), features and memory properties of the phyiscal device (so that examples can check against them)
	vkGetPhysicalDeviceProperties(physicalDevice, &deviceProperties);
	vkGetPhysicalDeviceFeatures(physicalDevice, &deviceFeatures);
	vkGetPhysicalDeviceMemoryProperties(physicalDevice, &deviceMemoryProperties);

	// Derived examples can override this to set actual features (based on above readings) to enable for logical device creation
	getEnabledFeatures();

	// Vulkan device creation
	// This is handled by a separate class that gets a logical device representation
	// and encapsulates functions related to a device
	vulkanDevice = new vks::VulkanDevice(physicalDevice);
	VkResult res = vulkanDevice->createLogicalDevice(enabledFeatures, enabledExtensions);
	if (res != VK_SUCCESS) {
		vks::tools::exitFatal("Could not create Vulkan device: \n" + vks::tools::errorString(res), "Fatal error", !benchmark.active);
	}
	device = vulkanDevice->logicalDevice;

	// Get a graphics queue from the device
	vkGetDeviceQueue(device, vulkanDevice->queueFamilyIndices.graphics, 0, &queue);

	// Find a suitable depth format
	VkBool32 validDepthFormat = vks::tools::getSupportedDepthFormat(physicalDevice, &depthFormat);
	assert(validDepthFormat);

	}
	
	UInt32 Device::GetMemoryTypeIndex(UInt32 typeBits, VkMemoryPropertyFlags properties)
	{
		for (uint32_t i = 0; i < m_deviceMemoryProperties.memoryTypeCount; i++)
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
		
		Utils::LOG_ERROR("GetMemoryTypeIndex");
	}
	
	VkCommandBuffer Device::GetCommandBuffer(bool beginRecord)
	{
		VkCommandBuffer cmdBuffer;

		VkCommandBufferAllocateInfo cmdBufAllocateInfo = VkFactory::CommandBufferAllocateInfo(m_cmdPool, VK_COMMAND_BUFFER_LEVEL_PRIMARY);
		Utils::VK_CHECK_RESULT(vkAllocateCommandBuffers(m_device, &cmdBufAllocateInfo, &cmdBuffer));

		if (beginRecord)
		{
			VkCommandBufferBeginInfo cmdBufInfo = VkFactory::CommandBufferBeginInfo();
			Utils::VK_CHECK_RESULT(vkBeginCommandBuffer(cmdBuffer, &cmdBufInfo));
		}

		return cmdBuffer;
	}
	
	void Device::FlushCommandBuffer(VkCommandBuffer commandBuffer)
	{
		assert(commandBuffer != VK_NULL_HANDLE);

		Utils::VK_CHECK_RESULT(vkEndCommandBuffer(commandBuffer));

		std::vector<VkCommandBuffer> commandBuffers;
		commandBuffers.push_back(commandBuffer);
		VkSubmitInfo submitInfo = VkFactory::SubmitInfo(commandBuffers);

		VkFenceCreateInfo fenceCreateInfo = VkFactory::FenceCreateInfo();
		VkFence fence;
		Utils::VK_CHECK_RESULT(vkCreateFence(m_device, &fenceCreateInfo, nullptr, &fence));

		Utils::VK_CHECK_RESULT(vkQueueSubmit(m_device, 1, &submitInfo, fence));
		VK_CHECK_RESULT(vkWaitForFences(m_device, 1, &fence, VK_TRUE, DEFAULT_FENCE_TIMEOUT));

		/**TODO, RAII */
		vkDestroyFence(m_device, fence, nullptr);
		vkFreeCommandBuffers(m_device, m_cmdPool, 1, &commandBuffer);
	}
}