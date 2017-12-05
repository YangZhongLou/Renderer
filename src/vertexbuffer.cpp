
#include "vertexbuffer.h"
#include "utils.h"
#include "device.h"

namespace Concise
{
	VertexBuffer::VertexBuffer(Device * device) : m_device(device)
	{
	}
	
	VertexBuffer::~VertexBuffer()
	{
		
	}
	
	void VertexBuffer::Submit(const std::vector<Vertex>& vertices)
	{
		struct StagingBuffer 
		{
			VkDeviceMemory memory;
			VkBuffer buffer;
		};
		
		StagingBuffer stagingVertices;
		
		/* TODO, refine here */
		VkBufferCreateInfo vertexBufferCreateInfo = {};
		vertexBufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		vertexBufferCreateInfo.size = vertices.size() * sizeof(Vertex);
		vertexBufferCreateInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
		
		Utils::VK_CHECK_RESULT(vkCreateBuffer(m_device->GetLogicalDevice(), &vertexBufferCreateInfo, nullptr, &stagingVertices.buffer));
		
		VkMemoryAllocateInfo memAlloc = {};
		memAlloc.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		VkMemoryRequirements memReqs;
		vkGetBufferMemoryRequirements(m_device->GetLogicalDevice(), stagingVertices.buffer, &memReqs);
		memAlloc.allocationSize = memReqs.size;
		
		memAlloc.memoryTypeIndex = m_device->GetMemoryTypeIndex(memReqs.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
		Utils::VK_CHECK_RESULT(vkAllocateMemory(m_device->GetLogicalDevice(), &memAlloc, nullptr, &stagingVertices.memory));
		
		void *data;
		Utils::VK_CHECK_RESULT(vkMapMemory(m_device->GetLogicalDevice(), stagingVertices.memory, 0, memAlloc.allocationSize, 0, &data));
		memcpy(data, verticesData.data(), vertexBufferSize);
		vkUnmapMemory(m_device->GetLogicalDevice(), stagingBuffers.vertices.memory);
		Utils::VK_CHECK_RESULT(vkBindBufferMemory(m_device->GetLogicalDevice(), stagingVertices.buffer, stagingVertices.memory, 0));
	
		vertexBufferCreateInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
		Utils::VK_CHECK_RESULT(vkCreateBuffer(m_device->GetLogicalDevice(), &vertexBufferCreateInfo, nullptr, &m_buffer));
		vkGetBufferMemoryRequirements(m_device->GetLogicalDevice(), m_buffer, &memReqs);
		memAlloc.allocationSize = memReqs.size;
		memAlloc.memoryTypeIndex = getMemoryTypeIndex(memReqs.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
		Utils::VK_CHECK_RESULT(vkAllocateMemory(m_device->GetLogicalDevice(), &memAlloc, nullptr, &m_memory));
		Utils::VK_CHECK_RESULT(vkBindBufferMemory(m_device->GetLogicalDevice(), m_buffer, m_memory, 0));
	}
}