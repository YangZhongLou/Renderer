#include "vertices.h"
#include "device.h"
#include "vkfactory.hpp"
#include "utils.h"

namespace Concise
{
	Vertices::Vertices(Device * device) : m_device(device)
	{
	}
	
	Vertices::~Vertices()
	{
		vkDestroyBuffer(m_device->GetLogicalDevice(), m_vertexBuffer.buffer, nullptr);
		vkFreeMemory(m_device->GetLogicalDevice(), m_vertexBuffer.memory, nullptr);

		vkDestroyBuffer(m_device->GetLogicalDevice(), m_indexBuffer.buffer, nullptr);
		vkFreeMemory(m_device->GetLogicalDevice(), m_indexBuffer.memory, nullptr);
	}
	
	/** refine it, delete duplicated codes */
	void Vertices::Submit(std::vector<Vertex> & vertexData, std::vector<UInt32> & indexData)
	{
		Buffer stagingVertices, staginIndices;
		
		/** vertex buffer */
		/** TODO, refine here, pre-allocate a chunk of memory */
		UInt32 vertexDataSize = static_cast<UInt32>(vertexData.size()) * sizeof(Vertex);
		UInt32 indexDataSize = static_cast<UInt32>(indexData.size()) * sizeof(UInt32);
		VkBufferCreateInfo vertexBufferCreateInfo = VkFactory::BufferCreateInfo(vertexDataSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT);

		VK_CHECK_RESULT(vkCreateBuffer(m_device->GetLogicalDevice(), &vertexBufferCreateInfo, nullptr, &stagingVertices.buffer));
		
		VkMemoryRequirements memReqs;
		vkGetBufferMemoryRequirements(m_device->GetLogicalDevice(), stagingVertices.buffer, &memReqs);
		
		VkMemoryAllocateInfo memAlloc = VkFactory::MemoryAllocateInfo(memReqs.size, 
			m_device->GetMemoryTypeIndex(memReqs.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT));

		VK_CHECK_RESULT(vkAllocateMemory(m_device->GetLogicalDevice(), &memAlloc, nullptr, &stagingVertices.memory));
		
		void *data;
		VK_CHECK_RESULT(vkMapMemory(m_device->GetLogicalDevice(), stagingVertices.memory, 0, memAlloc.allocationSize, 0, &data));
		memcpy(data, vertexData.data(), vertexDataSize);
		vkUnmapMemory(m_device->GetLogicalDevice(), stagingVertices.memory);
		VK_CHECK_RESULT(vkBindBufferMemory(m_device->GetLogicalDevice(), stagingVertices.buffer, stagingVertices.memory, 0));
		
		/** detination buffer */
		vertexBufferCreateInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
		VK_CHECK_RESULT(vkCreateBuffer(m_device->GetLogicalDevice(), &vertexBufferCreateInfo, nullptr, &m_vertexBuffer.buffer));
		vkGetBufferMemoryRequirements(m_device->GetLogicalDevice(), m_vertexBuffer.buffer, &memReqs);
		memAlloc.allocationSize = memReqs.size;
		memAlloc.memoryTypeIndex = m_device->GetMemoryTypeIndex(memReqs.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
		VK_CHECK_RESULT(vkAllocateMemory(m_device->GetLogicalDevice(), &memAlloc, nullptr, &m_vertexBuffer.memory));
		VK_CHECK_RESULT(vkBindBufferMemory(m_device->GetLogicalDevice(), m_vertexBuffer.buffer, m_vertexBuffer.memory, 0));
		
		/** index buffer */
		m_indexCount = static_cast<UInt32>(indexData.size());
		VkBufferCreateInfo indexBufferCreateInfo = VkFactory::BufferCreateInfo(indexDataSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT);
		VK_CHECK_RESULT(vkCreateBuffer(m_device->GetLogicalDevice(), &indexBufferCreateInfo, nullptr, &staginIndices.buffer));
		vkGetBufferMemoryRequirements(m_device->GetLogicalDevice(), staginIndices.buffer, &memReqs);
		memAlloc.allocationSize = memReqs.size;
		memAlloc.memoryTypeIndex = m_device->GetMemoryTypeIndex(memReqs.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
		
		VK_CHECK_RESULT(vkAllocateMemory(m_device->GetLogicalDevice(), &memAlloc, nullptr, &staginIndices.memory));
		VK_CHECK_RESULT(vkMapMemory(m_device->GetLogicalDevice(), staginIndices.memory, 0, indexDataSize, 0, &data));
		memcpy(data, indexData.data(), indexDataSize);
		vkUnmapMemory(m_device->GetLogicalDevice(), staginIndices.memory);
		VK_CHECK_RESULT(vkBindBufferMemory(m_device->GetLogicalDevice(), staginIndices.buffer, staginIndices.memory, 0));

		/** destination buffer */
		indexBufferCreateInfo.usage = VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
		VK_CHECK_RESULT(vkCreateBuffer(m_device->GetLogicalDevice(), &indexBufferCreateInfo, nullptr, &m_indexBuffer.buffer));
		vkGetBufferMemoryRequirements(m_device->GetLogicalDevice(), m_indexBuffer.buffer, &memReqs);
		memAlloc.allocationSize = memReqs.size;
		memAlloc.memoryTypeIndex = m_device->GetMemoryTypeIndex(memReqs.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
		VK_CHECK_RESULT(vkAllocateMemory(m_device->GetLogicalDevice(), &memAlloc, nullptr, &m_indexBuffer.memory));
		VK_CHECK_RESULT(vkBindBufferMemory(m_device->GetLogicalDevice(), m_indexBuffer.buffer, m_indexBuffer.memory, 0));

		VkCommandBuffer copyCmd = m_device->GetCommandBuffer(true);
		
		VkBufferCopy copyRegion {};

		copyRegion.size = vertexDataSize;
		vkCmdCopyBuffer(copyCmd, stagingVertices.buffer, m_vertexBuffer.buffer, 1, &copyRegion);
		
		copyRegion.size = indexDataSize;
		vkCmdCopyBuffer(copyCmd, staginIndices.buffer, m_indexBuffer.buffer, 1, &copyRegion);

		m_device->FlushCommandBuffer(copyCmd);

		/** TODO, RAII */
		vkDestroyBuffer(m_device->GetLogicalDevice(), stagingVertices.buffer, nullptr);
		vkFreeMemory(m_device->GetLogicalDevice(), stagingVertices.memory, nullptr);
		vkDestroyBuffer(m_device->GetLogicalDevice(), staginIndices.buffer, nullptr);
		vkFreeMemory(m_device->GetLogicalDevice(), staginIndices.memory, nullptr);
	}
}