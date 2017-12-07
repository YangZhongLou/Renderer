

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "uniforms.h"
#include "vkfactory.h"
#include "utils.h"
#include "device.h"

namespace Concise
{
	Uniforms::Uniforms(Device * device) : m_device(device)
	{
	}
	
	Uniforms::~Uniforms()
	{
	}
	
	void Uniforms::Init()
	{
		VkBufferCreateInfo bufferInfo = VkFactory::BufferCreateInfo(sizeof(m_vsData), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT);;
		Utils::VK_CHECK_RESULT(vkCreateBuffer(m_device->GetLogicalDevice(), &bufferInfo, nullptr, &m_vsBuffer.buffer));

		VkMemoryRequirements memReqs;
		vkGetBufferMemoryRequirements(m_device->GetLogicalDevice(), m_vsBuffer.buffer, &memReqs);
		
		VkMemoryAllocateInfo allocInfo = VkFactory::MemoryAllocateInfo(memReqs.size, 
			m_device->GetMemoryTypeIndex(memReqs.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT));
		Utils::VK_CHECK_RESULT(vkAllocateMemory(m_device->GetLogicalDevice(), &allocInfo, nullptr, &(m_vsBuffer.memory)));
		Utils::VK_CHECK_RESULT(vkBindBufferMemory(m_device->GetLogicalDevice(), m_vsBuffer.buffer, m_vsBuffer.memory, 0));
		
		m_vsBuffer.descriptor.buffer = m_vsBuffer.buffer;
		m_vsBuffer.descriptor.offset = 0;
		m_vsBuffer.descriptor.range = sizeof(m_vsData);
	}
	
	void Uniforms::UpdateVS()
	{
		m_vsData.projectionMatrix = glm::perspective(glm::radians(60.0f), (float)width / (float)height, 0.1f, 256.0f);

		m_vsData.viewMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, zoom));

		m_vsData.modelMatrix = glm::mat4(1.0f);
		m_vsData.modelMatrix = glm::rotate(m_vsData.modelMatrix, glm::radians(rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
		m_vsData.modelMatrix = glm::rotate(m_vsData.modelMatrix, glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
		m_vsData.modelMatrix = glm::rotate(m_vsData.modelMatrix, glm::radians(rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));

		void * data;
		Utils::VK_CHECK_RESULT(vkMapMemory(m_device->GetLogicalDevice(), m_vsBuffer.memory, 0, sizeof(m_vsData), 0, (void **)&data));
		memcpy(data, &m_vsData, sizeof(m_vsData));
		vkUnmapMemory(m_device->GetLogicalDevice(), m_vsBuffer.memory);
	}
}