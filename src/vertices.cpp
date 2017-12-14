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
	}
	
	/** refine it, delete duplicated codes */
	void Vertices::Submit(std::vector<Vertex>& vertexData, std::vector<UInt32>& indexData)
	{
		Buffer stagingVertices, staginIndices;
		
		/** vertex buffer */
		/** TODO, refine here, pre-allocate a chunk of memory */
		UInt32 vertexDataSize = vertexData.size() * sizeof(Vertex);
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
		m_indexCount = indexData.size();
		VkBufferCreateInfo indexBufferCreateInfo = VkFactory::BufferCreateInfo(indexData.size(), VK_BUFFER_USAGE_TRANSFER_SRC_BIT);
		VK_CHECK_RESULT(vkCreateBuffer(m_device->GetLogicalDevice(), &indexBufferCreateInfo, nullptr, &staginIndices.buffer));
		vkGetBufferMemoryRequirements(m_device->GetLogicalDevice(), staginIndices.buffer, &memReqs);
		memAlloc.allocationSize = memReqs.size;
		memAlloc.memoryTypeIndex = m_device->GetMemoryTypeIndex(memReqs.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
		
		VK_CHECK_RESULT(vkAllocateMemory(m_device->GetLogicalDevice(), &memAlloc, nullptr, &staginIndices.memory));
		VK_CHECK_RESULT(vkMapMemory(m_device->GetLogicalDevice(), staginIndices.memory, 0, indexData.size(), 0, &data));
		memcpy(data, indexData.data(), indexData.size());
		vkUnmapMemory(device, staginIndices.memory);
		VK_CHECK_RESULT(vkBindBufferMemory(device, staginIndices.buffer, staginIndices.memory, 0));

		/** destination buffer */
		indexBufferCreateInfo.usage = VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
		VK_CHECK_RESULT(vkCreateBuffer(m_device->GetLogicalDevice(), &indexBufferCreateInfo, nullptr, &m_indexBuffer.buffer));
		vkGetBufferMemoryRequirements(m_device->GetLogicalDevice(), m_indexBuffer.buffer, &memReqs);
		memAlloc.allocationSize = memReqs.size;
		memAlloc.memoryTypeIndex = m_device->GetMemoryTypeIndex(memReqs.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
		VK_CHECK_RESULT(vkAllocateMemory(device, &memAlloc, nullptr, &m_indexBuffer.memory));
		VK_CHECK_RESULT(vkBindBufferMemory(device, m_indexBuffer.buffer, m_indexBuffer.memory, 0));

		VkCommandBuffer copyCmd = m_device->GetCommandBuffer(true);
		
		VkBufferCopy copyRegion = {};

		copyRegion.size = vertexDataSize;
		vkCmdCopyBuffer(copyCmd, stagingVertices.buffer, m_vertexBuffer.buffer, 1, &copyRegion);
		
		copyRegion.size = indexData.size();
		vkCmdCopyBuffer(copyCmd, staginIndices.buffer, m_indexBuffer.buffer, 1, &copyRegion);

		m_device->FlushCommandBuffer(copyCmd);

		/** TODO, RAII */
		vkDestroyBuffer(device, stagingVertices.buffer, nullptr);
		vkFreeMemory(device, stagingVertices.memory, nullptr);
		vkDestroyBuffer(device, staginIndices.buffer, nullptr);
		vkFreeMemory(device, staginIndices.memory, nullptr);
	}
}