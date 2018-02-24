#pragma once

namespace Concise
{	
	struct Buffer
	{
		VkBuffer buffer = VK_NULL_HANDLE;
		VkDeviceMemory memory = VK_NULL_HANDLE;
		VkDescriptorBufferInfo descriptor;
		VkDeviceSize size = 0;
		VkDeviceSize alignment = 0;
		
		void* mapped = nullptr;

		VkBufferUsageFlags usageFlags;
		VkMemoryPropertyFlags memoryPropertyFlags;
		
		Buffer()
		{}
		
		~Buffer()
		{
			Destroy();
		}

		VkResult Map(VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0)
		{
			return vkMapMemory(Device::Instance().GetLogicalDevice(), memory, offset, size, 0, &mapped);
		}

		void Unmap()
		{
			if (mapped)
			{
				vkUnmapMemory(Device::Instance().GetLogicalDevice(), memory);
				mapped = nullptr;
			}
		}

		VkResult Bind(VkDeviceSize offset = 0)
		{
			return vkBindBufferMemory(Device::Instance().GetLogicalDevice(), buffer, memory, offset);
		}

		void SetupDescriptor(VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0)
		{
			descriptor.offset = offset;
			descriptor.buffer = buffer;
			descriptor.range = size;
		}

		inline void CopyTo(void* data, VkDeviceSize size)
		{
			assert(mapped);
			memcpy(mapped, data, size);
		}

		VkResult Flush(VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0)
		{
			VkMappedMemoryRange mappedRange = {};
			mappedRange.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
			mappedRange.memory = memory;
			mappedRange.offset = offset;
			mappedRange.size = size;
			return vkFlushMappedMemoryRanges(Device::Instance().GetLogicalDevice(), 1, &mappedRange);
		}

		VkResult Invalidate(VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0)
		{
			VkMappedMemoryRange mappedRange = {};
			mappedRange.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
			mappedRange.memory = memory;
			mappedRange.offset = offset;
			mappedRange.size = size;
			return vkInvalidateMappedMemoryRanges(Device::Instance().GetLogicalDevice(), 1, &mappedRange);
		}

		void Destroy()
		{
			if (buffer)
			{
				vkDestroyBuffer(Device::Instance().GetLogicalDevice(), buffer, nullptr);
			}
			if (memory)
			{
				vkFreeMemory(Device::Instance().GetLogicalDevice(), memory, nullptr);
			}
		}
	};
}