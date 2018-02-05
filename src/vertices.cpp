#include "vertices.h"
#include "device.h"
#include "vk_factory.hpp"
#include "utils.h"
#include "buffer.hpp"

namespace Concise
{
	Vertices::Vertices(Device * device) : m_device(device)
	{
		m_vertexDataOffset = 0;
		
		m_vertexBuffer = new Buffer;
		m_vertexBuffer->device = m_device->GetLogicalDevice();
		m_vertexStagingBuffer = new Buffer;
		m_vertexStagingBuffer->device = m_device->GetLogicalDevice();
		
		m_indexStagingBuffer = new Buffer;
		m_indexStagingBuffer->device = m_device->GetLogicalDevice();
		m_indexBuffer = new Buffer;
		m_indexBuffer->device = m_device->GetLogicalDevice();
	}
	
	Vertices::~Vertices()
	{
		SAFE_DELETE(m_vertexBuffer);
		SAFE_DELETE(m_vertexStagingBuffer);
		SAFE_DELETE(m_indexBuffer);
		SAFE_DELETE(m_indexStagingBuffer);
	}
	
	void Vertices::Init()
	{
		/** vertex buffer */
		VkBufferCreateInfo vertexBufferCreateInfo = VkFactory::BufferCreateInfo(DEFAULT_VERTEX_DATA_SIZE,  VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT);
		VK_CHECK_RESULT(vkCreateBuffer(m_device->GetLogicalDevice(), &vertexBufferCreateInfo, nullptr, &m_vertexBuffer->buffer));
		VkMemoryRequirements memReqs;
		vkGetBufferMemoryRequirements(m_device->GetLogicalDevice(), m_vertexBuffer->buffer, &memReqs);
		VkMemoryAllocateInfo memAlloc = VkFactory::MemoryAllocateInfo(memReqs.size, 
		m_device->GetMemoryTypeIndex(memReqs.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT));
		VK_CHECK_RESULT(vkAllocateMemory(m_device->GetLogicalDevice(), &memAlloc, nullptr, &m_vertexBuffer->memory));
		VK_CHECK_RESULT(vkBindBufferMemory(m_device->GetLogicalDevice(), m_vertexBuffer->buffer, m_vertexBuffer->memory, 0));

		/** vertex staging buffer */
		vertexBufferCreateInfo = VkFactory::BufferCreateInfo(DEFAULT_STAGING_BUFFER_SIZE, VK_BUFFER_USAGE_TRANSFER_SRC_BIT);
		VK_CHECK_RESULT(vkCreateBuffer(m_device->GetLogicalDevice(), &vertexBufferCreateInfo, nullptr, &m_vertexStagingBuffer->buffer));
		vkGetBufferMemoryRequirements(m_device->GetLogicalDevice(), m_vertexStagingBuffer->buffer, &memReqs);
		memAlloc.allocationSize = memReqs.size;
		memAlloc.memoryTypeIndex = m_device->GetMemoryTypeIndex(memReqs.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
		VK_CHECK_RESULT(vkAllocateMemory(m_device->GetLogicalDevice(), &memAlloc, nullptr, &m_vertexStagingBuffer->memory));
		VK_CHECK_RESULT(vkBindBufferMemory(m_device->GetLogicalDevice(), m_vertexStagingBuffer->buffer, m_vertexStagingBuffer->memory, 0));

		/** index buffer */
		VkBufferCreateInfo indexBufferCreateInfo = VkFactory::BufferCreateInfo(DEFAULT_INDEX_DATA_SIZE, VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT);
		VK_CHECK_RESULT(vkCreateBuffer(m_device->GetLogicalDevice(), &indexBufferCreateInfo, nullptr, &m_indexBuffer->buffer));
		vkGetBufferMemoryRequirements(m_device->GetLogicalDevice(), m_indexBuffer->buffer, &memReqs);
		memAlloc.allocationSize = memReqs.size;
		memAlloc.memoryTypeIndex = m_device->GetMemoryTypeIndex(memReqs.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
		VK_CHECK_RESULT(vkAllocateMemory(m_device->GetLogicalDevice(), &memAlloc, nullptr, &m_indexBuffer->memory));
		VK_CHECK_RESULT(vkBindBufferMemory(m_device->GetLogicalDevice(), m_indexBuffer->buffer, m_indexBuffer->memory, 0));

		/** index staging buffer */
		indexBufferCreateInfo = VkFactory::BufferCreateInfo(indexDataSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT);
		VK_CHECK_RESULT(vkCreateBuffer(m_device->GetLogicalDevice(), &indexBufferCreateInfo, nullptr, &m_indexStagingBuffer->buffer));
		vkGetBufferMemoryRequirements(m_device->GetLogicalDevice(), m_indexStagingBuffer->buffer, &memReqs);
		memAlloc.allocationSize = memReqs.size;
		memAlloc.memoryTypeIndex = m_device->GetMemoryTypeIndex(memReqs.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
		VK_CHECK_RESULT(vkAllocateMemory(m_device->GetLogicalDevice(), &memAlloc, nullptr, &m_indexStagingBuffer->memory));
		VK_CHECK_RESULT(vkBindBufferMemory(m_device->GetLogicalDevice(), m_indexStagingBuffer->buffer, m_indexStagingBuffer->memory, 0));
	}
	
	/** TODO, when buffer is full, flush data and reset buffer-data-offset */
	void Vertices::Submit(std::vector<Vertex> & vertexData, std::vector<UInt32> & indexData)
	{		
		UInt32 vertexDataSize = static_cast<UInt32>(vertexData.size()) * sizeof(Vertex);
		UInt32 indexDataSize = static_cast<UInt32>(indexData.size()) * sizeof(UInt32);
		m_indexCount = static_cast<UInt32>(indexData.size());

		/** copy data to staging buffer */
		void *data;
		VK_CHECK_RESULT(vkMapMemory(m_device->GetLogicalDevice(), m_vertexStagingBuffer.memory, 0, vertexDataSize, 0, &data));
		memcpy(data, vertexData.data(), vertexDataSize);
		vkUnmapMemory(m_device->GetLogicalDevice(), m_vertexStagingBuffer.memory);
		
		/** copy data to staging buffer */
		VK_CHECK_RESULT(vkMapMemory(m_device->GetLogicalDevice(), staginIndices.memory, 0, indexDataSize, 0, &data));
		memcpy(data, indexData.data(), indexDataSize);
		vkUnmapMemory(m_device->GetLogicalDevice(), staginIndices.memory);

		VkCommandBuffer copyCmd = m_device->GetCommandBuffer(true);
		
		VkBufferCopy copyRegion {};

		copyRegion.dstOffset = m_vertexDataOffset;
		copyRegion.size = vertexDataSize;
		vkCmdCopyBuffer(copyCmd, m_vertexStagingBuffer.buffer, m_vertexBuffer->buffer, 1, &copyRegion);
		m_vertexDataOffset += vertexDataSize;
		
		copyRegion.dstOffset = m_indexDataOffset;
		copyRegion.size = indexDataSize;
		vkCmdCopyBuffer(copyCmd, m_indexStagingBuffer.buffer, m_indexBuffer->buffer, 1, &copyRegion);
		m_indexDataOffset += indexDataSize;

		m_device->FlushCommandBuffer(copyCmd);
	}
}