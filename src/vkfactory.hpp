/*
* This code is licensed under the MIT license (MIT) (http://opensource.org/licenses/MIT)
*/
#pragma once

#include <vector>
#include <vulkan/vulkan.h>
#include "types.h"

namespace Consice
{
	namespace VkFactory
	{
		inline VkSemaphoreCreateInfo SemaphoreCreateInfo()
		{
			VkSemaphoreCreateInfo semaphoreCreateInfo {};
			semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
			semaphoreCreateInfo.pNext = nullptr;
			
			return semaphoreCreateInfo;
		}
		
		inline VkFenceCreateInfo FenceCreateInfo(VkFenceCreateFlags flags = 0)
		{
			VkFenceCreateInfo fenceCreateInfo = {};
			fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
			fenceCreateInfo.flags = flags;
			
			return fenceCreateInfo;
		}
		
		inline VkSubmitInfo SubmitInfo(std::vector<VkCommandBuffer>& commandBuffers)
		{
			VkSubmitInfo submitInfo = {};
			submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
			submitInfo.commandBufferCount = commandBuffers.size();
			submitInfo.pCommandBuffers = &commandBuffers.data();
			
			return submitInfo;
		}
		
		inline VkMemoryAllocateInfo MemoryAllocateInfo(VkDeviceSize size, UInt32 memoryTypeIndex)
		{
			VkMemoryAllocateInfo memAlloc = {};
			memAlloc.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
			memAlloc.allocationSize = size;
			memAlloc.memoryTypeIndex = memoryTypeIndex;
			
			return memAlloc;
		}
		
		inline VkBufferCreateInfo BufferCreateInfo(VkDeviceSize size, VkBufferUsageFlags usage)
		{
			VkBufferCreateInfo bufferCreateInfo {};
			bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
			bufferCreateInfo.size = size;
			bufferCreateInfo.usage = usage;
			
			return bufferCreateInfo;
		}
		
		inline VkCommandBufferBeginInfo CommandBufferBeginInfo()
		{
			VkCommandBufferBeginInfo cmdBufferBeginInfo {};
			cmdBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
			
			return cmdBufferBeginInfo;
		}
		
		
	}
}