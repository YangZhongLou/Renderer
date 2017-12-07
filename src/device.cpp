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