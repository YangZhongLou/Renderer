#include "vertices.h"
#include "device.h"
#include "vk_factory.hpp"
#include "buffer.hpp"
#include "defines.h"

namespace Concise
{
	Vertices::Vertices()
	{
		m_vertexDataOffset = 0;
		
		m_vertexBuffer = new Buffer;
		m_vertexStagingBuffer = new Buffer;
		
		m_indexStagingBuffer = new Buffer;
		m_indexBuffer = new Buffer;

		InitBuffers();
	}
	
	Vertices::~Vertices()
	{
		SAFE_DELETE(m_vertexBuffer);
		SAFE_DELETE(m_vertexStagingBuffer);
		SAFE_DELETE(m_indexBuffer);
		SAFE_DELETE(m_indexStagingBuffer);
	}
	
	void Vertices::InitBuffers()
	{
		/** vertex buffer */
		VkBufferCreateInfo vertexBufferCreateInfo = 
			VkFactory::BufferCreateInfo(DEFAULT_VERTEX_DATA_SIZE,  
				VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT);
		VK_CHECK_RESULT(vkCreateBuffer(Device::Instance().GetLogicalDevice(), 
			&vertexBufferCreateInfo, nullptr, &m_vertexBuffer->buffer));
		VkMemoryRequirements memReqs;
		vkGetBufferMemoryRequirements(Device::Instance().GetLogicalDevice(), 
			m_vertexBuffer->buffer, &memReqs);
		VkMemoryAllocateInfo memAlloc = VkFactory::MemoryAllocateInfo(memReqs.size, 
		Device::Instance().GetMemoryTypeIndex(memReqs.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT));
		VK_CHECK_RESULT(vkAllocateMemory(Device::Instance().GetLogicalDevice(), 
			&memAlloc, nullptr, &m_vertexBuffer->memory));
		VK_CHECK_RESULT(vkBindBufferMemory(Device::Instance().GetLogicalDevice(), 
			m_vertexBuffer->buffer, m_vertexBuffer->memory, 0));

		/** vertex staging buffer */
		vertexBufferCreateInfo = VkFactory::BufferCreateInfo(DEFAULT_STAGING_BUFFER_SIZE, VK_BUFFER_USAGE_TRANSFER_SRC_BIT);
		VK_CHECK_RESULT(vkCreateBuffer(Device::Instance().GetLogicalDevice(), 
			&vertexBufferCreateInfo, nullptr, &m_vertexStagingBuffer->buffer));
		vkGetBufferMemoryRequirements(Device::Instance().GetLogicalDevice(), 
			m_vertexStagingBuffer->buffer, &memReqs);
		memAlloc.allocationSize = memReqs.size;
		memAlloc.memoryTypeIndex = Device::Instance().GetMemoryTypeIndex(memReqs.memoryTypeBits, 
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
		VK_CHECK_RESULT(vkAllocateMemory(Device::Instance().GetLogicalDevice(), 
			&memAlloc, nullptr, &m_vertexStagingBuffer->memory));
		VK_CHECK_RESULT(vkBindBufferMemory(Device::Instance().GetLogicalDevice(), 
			m_vertexStagingBuffer->buffer, m_vertexStagingBuffer->memory, 0));

		/** index buffer */
		VkBufferCreateInfo indexBufferCreateInfo = 
			VkFactory::BufferCreateInfo(DEFAULT_INDEX_DATA_SIZE, 
				VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT);
		VK_CHECK_RESULT(vkCreateBuffer(Device::Instance().GetLogicalDevice(), 
			&indexBufferCreateInfo, nullptr, &m_indexBuffer->buffer));
		vkGetBufferMemoryRequirements(Device::Instance().GetLogicalDevice(), 
			m_indexBuffer->buffer, &memReqs);
		memAlloc.allocationSize = memReqs.size;
		memAlloc.memoryTypeIndex = Device::Instance().GetMemoryTypeIndex(memReqs.memoryTypeBits, 
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
		VK_CHECK_RESULT(vkAllocateMemory(Device::Instance().GetLogicalDevice(), 
			&memAlloc, nullptr, &m_indexBuffer->memory));
		VK_CHECK_RESULT(vkBindBufferMemory(Device::Instance().GetLogicalDevice(), 
			m_indexBuffer->buffer, m_indexBuffer->memory, 0));

		/** index staging buffer */
		indexBufferCreateInfo = VkFactory::BufferCreateInfo(DEFAULT_STAGING_BUFFER_SIZE, VK_BUFFER_USAGE_TRANSFER_SRC_BIT);
		VK_CHECK_RESULT(vkCreateBuffer(Device::Instance().GetLogicalDevice(), 
			&indexBufferCreateInfo, nullptr, &m_indexStagingBuffer->buffer));
		vkGetBufferMemoryRequirements(Device::Instance().GetLogicalDevice(), 
			m_indexStagingBuffer->buffer, &memReqs);
		memAlloc.allocationSize = memReqs.size;
		memAlloc.memoryTypeIndex = Device::Instance().GetMemoryTypeIndex(memReqs.memoryTypeBits, 
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
		VK_CHECK_RESULT(vkAllocateMemory(Device::Instance().GetLogicalDevice(), 
			&memAlloc, nullptr, &m_indexStagingBuffer->memory));
		VK_CHECK_RESULT(vkBindBufferMemory(Device::Instance().GetLogicalDevice(), 
			m_indexStagingBuffer->buffer, m_indexStagingBuffer->memory, 0));
	}

	void Vertices::Submit(std::vector<float> & verticesData, std::vector<UInt32> & indicesData)
	{

	}
	
	/** TODO, when buffer is full, flush data and reset buffer-data-offset */
	void Vertices::Submit(std::vector<Vertex> & vertexData, std::vector<UInt32> & indexData)
	{		
		UInt32 vertexDataSize = static_cast<UInt32>(vertexData.size()) * sizeof(Vertex);
		UInt32 indexDataSize = static_cast<UInt32>(indexData.size()) * sizeof(UInt32);
		m_indexCount = static_cast<UInt32>(indexData.size());

		/** copy data to staging buffer */
		void *data;
		VK_CHECK_RESULT(vkMapMemory(Device::Instance().GetLogicalDevice(), 
			m_vertexStagingBuffer->memory, 0, vertexDataSize, 0, &data));
		memcpy(data, vertexData.data(), vertexDataSize);
		vkUnmapMemory(Device::Instance().GetLogicalDevice(), 
			m_vertexStagingBuffer->memory);
		
		/** copy data to staging buffer */
		VK_CHECK_RESULT(vkMapMemory(Device::Instance().GetLogicalDevice(),
			m_indexStagingBuffer->memory, 0, indexDataSize, 0, &data));
		memcpy(data, indexData.data(), indexDataSize);
		vkUnmapMemory(Device::Instance().GetLogicalDevice(),
			m_indexStagingBuffer->memory);

		VkCommandBuffer copyCmd = Device::Instance().GetCommandBuffer(true);
		
		VkBufferCopy copyRegion {};

		copyRegion.dstOffset = m_vertexDataOffset;
		copyRegion.size = vertexDataSize;
		vkCmdCopyBuffer(copyCmd, m_vertexStagingBuffer->buffer, m_vertexBuffer->buffer, 1, &copyRegion);
		m_vertexDataOffset += vertexDataSize;
		
		copyRegion.dstOffset = m_indexDataOffset;
		copyRegion.size = indexDataSize;
		vkCmdCopyBuffer(copyCmd, m_indexStagingBuffer->buffer, m_indexBuffer->buffer, 1, &copyRegion);
		m_indexDataOffset += indexDataSize;

		Device::Instance().FlushCommandBuffer(copyCmd);
	}

	/** 
	* TODO, make sure vertices data stream layout is the same as vertexLayout.
	*/
	void Vertices::SetInputLayout(VertexLayout & vertexLayout)
	{
		m_verticesInput.bindingDescriptions.resize(1);
		m_verticesInput.bindingDescriptions[0] =
			VkFactory::VertexInputBindingDescription(
				VERTEX_BUFFER_BIND_ID,
				vertexLayout.Stride(),
				VK_VERTEX_INPUT_RATE_VERTEX);

		m_verticesInput.attributeDescriptions.resize(vertexLayout.components.size());

		int count = 0;
		int offset = 0;
		for (auto& component : vertexLayout.components)
		{
			switch (component)
			{
			case VERTEX_COMPONENT_POSITION:
				m_verticesInput.attributeDescriptions[count] =
					VkFactory::VertexInputAttributeDescription(VERTEX_BUFFER_BIND_ID,
						count,
						VK_FORMAT_R32G32B32_SFLOAT,
						offset);
				offset += 3 * sizeof(float);
				break;
			case VERTEX_COMPONENT_NORMAL:
				m_verticesInput.attributeDescriptions[count] =
					VkFactory::VertexInputAttributeDescription(VERTEX_BUFFER_BIND_ID,
						count,
						VK_FORMAT_R32G32B32_SFLOAT,
						offset);
				offset += 3 * sizeof(float);
				break;
			case VERTEX_COMPONENT_UV:
				m_verticesInput.attributeDescriptions[count] =
					VkFactory::VertexInputAttributeDescription(VERTEX_BUFFER_BIND_ID,
						count,
						VK_FORMAT_R32G32_SFLOAT,
						offset);
				offset += 2 * sizeof(float);
				break;
			case VERTEX_COMPONENT_COLOR:
				m_verticesInput.attributeDescriptions[count] =
					VkFactory::VertexInputAttributeDescription(VERTEX_BUFFER_BIND_ID,
						count,
						VK_FORMAT_R32G32B32_SFLOAT,
						offset);
				offset += 3 * sizeof(float);
				break;
			case VERTEX_COMPONENT_TANGENT:
				m_verticesInput.attributeDescriptions[count] =
					VkFactory::VertexInputAttributeDescription(VERTEX_BUFFER_BIND_ID,
						count,
						VK_FORMAT_R32G32B32_SFLOAT,
						offset);
				offset += 3 * sizeof(float);
				break;
			case VERTEX_COMPONENT_BITANGENT:
				m_verticesInput.attributeDescriptions[count] =
					VkFactory::VertexInputAttributeDescription(VERTEX_BUFFER_BIND_ID,
						count,
						VK_FORMAT_R32G32B32_SFLOAT,
						offset);
				offset += 3 * sizeof(float);
				break;
				// Dummy components for padding
			case VERTEX_COMPONENT_DUMMY_FLOAT:
				break;
			case VERTEX_COMPONENT_DUMMY_VEC4:
				break;
			};
			count++;
		}
		assert(count == m_verticesInput.attributeDescriptions.size() - 1);
		m_verticesInput.inputState =
			VkFactory::PipelineVertexInputStateCreateInfo(
				m_verticesInput.bindingDescriptions,
				m_verticesInput.attributeDescriptions
			);
	}
}