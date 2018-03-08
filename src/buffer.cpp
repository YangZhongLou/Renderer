#include "defines.h"
#include "buffer.h"
#include "vk_factory.hpp"
#include "device.h"


namespace Concise
{
	Buffer::Buffer(
		VkBufferUsageFlags usageFlags,
		VkMemoryPropertyFlags memoryPropertyFlags,
		VkDeviceSize size,
		void *data = nullptr)
	{
		VkBufferCreateInfo bufferCreateInfo = VkFactory::BufferCreateInfo(size, usageFlags);
		VK_CHECK_RESULT(vkCreateBuffer(LogicalDevice, &bufferCreateInfo, nullptr, &buffer));

		VkMemoryRequirements memReqs;
		vkGetBufferMemoryRequirements(LogicalDevice, buffer, &memReqs);

		VkMemoryAllocateInfo memAlloc = VkFactory::MemoryAllocateInfo(
			memReqs.size,
			Device::Instance().GetMemoryTypeIndex(memReqs.memoryTypeBits, memoryPropertyFlags));
		VK_CHECK_RESULT(vkAllocateMemory(LogicalDevice, &memAlloc, nullptr, &memory));

		alignment = memReqs.alignment;
		size = memAlloc.allocationSize;
		usageFlags = usageFlags;
		memoryPropertyFlags = memoryPropertyFlags;

		if (data != nullptr)
		{
			VK_CHECK_RESULT(Map());
			memcpy(mapped, data, size);
			Unmap();
		}

		SetupDescriptor();

		Bind();
	}
}