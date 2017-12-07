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
		inline VkRect2D Scissor(Int32 width, Int32 height, Int32 offsetX = 0, Int32 offsetY = 0)
		{
			VkRect2D scissor = {};
			scissor.extent.width = width;
			scissor.extent.height = height;
			scissor.offset.x = offsetX;
			scissor.offset.y = offsetY;
			
			return scissor;
		}
		
		inline VkViewport ViewPort(float width, float height, float minDepth, float maxDepth)
		{
			VkViewport viewport = {};
			viewport.height = height;
			viewport.width = width;
			viewport.minDepth = 0.0f;
			viewport.maxDepth = 1.0f;
			
			return viewport;
		}
		
		inline VkRenderPassBeginInfo RenderPassBeginInfo()
		{
			VkRenderPassBeginInfo renderPassBeginInfo = {};
			renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
			renderPassBeginInfo.pNext = nullptr;
			
			return renderPassBeginInfo;
		}
		
		inline VkCommandBufferAllocateInfo CommandBufferAllocateInfo(VkCommandPool commandPool, VkCommandBufferLevel level)
		{
			VkCommandBufferAllocateInfo cmdBufAllocateInfo = {};
			cmdBufAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
			cmdBufAllocateInfo.commandPool = commandPool;
			cmdBufAllocateInfo.level = level;
			cmdBufAllocateInfo.commandBufferCount = 1;
			
			return cmdBufAllocateInfo;
		}
		
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