#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "uniforms.h"
#include "vk_factory.hpp"
#include "utils.h"
#include "device.h"
#include "inputs.h"

namespace Concise
{
	Uniforms::Uniforms()
	{
		VkBufferCreateInfo bufferInfo = VkFactory::BufferCreateInfo(sizeof(m_vsData), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT);;
		VK_CHECK_RESULT(vkCreateBuffer(Device::Instance().GetLogicalDevice(), &bufferInfo, nullptr, &m_vsBuffer.buffer));

		VkMemoryRequirements memReqs;
		vkGetBufferMemoryRequirements(Device::Instance().GetLogicalDevice(), m_vsBuffer.buffer, &memReqs);
		
		VkMemoryAllocateInfo allocInfo = VkFactory::MemoryAllocateInfo(memReqs.size, 
			Device::Instance().GetMemoryTypeIndex(memReqs.memoryTypeBits, 
				VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT));
		VK_CHECK_RESULT(vkAllocateMemory(Device::Instance().GetLogicalDevice(), &allocInfo, nullptr, &(m_vsBuffer.memory)));
		VK_CHECK_RESULT(vkBindBufferMemory(Device::Instance().GetLogicalDevice(), m_vsBuffer.buffer, m_vsBuffer.memory, 0));
		
		m_vsBuffer.descriptor.buffer = m_vsBuffer.buffer;
		m_vsBuffer.descriptor.offset = 0;
		m_vsBuffer.descriptor.range = sizeof(m_vsData);		
	}
	
	Uniforms::~Uniforms()
	{
		vkDestroyBuffer(Device::Instance().GetLogicalDevice(), m_vsBuffer.buffer, nullptr);
		vkFreeMemory(Device::Instance().GetLogicalDevice(), m_vsBuffer.memory, nullptr);
	}
	
	void Uniforms::UpdateVS(UInt32 width, UInt32 height)
	{
		m_vsData.projectionMatrix = glm::perspective(glm::radians(60.0f), (float)width / (float)height, 0.1f, 256.0f);

		m_vsData.viewMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, Inputs::Instance().GetZoom()));

		m_vsData.modelMatrix = glm::mat4(1.0f);
		m_vsData.modelMatrix = glm::rotate(m_vsData.modelMatrix, 
			glm::radians(Inputs::Instance().GetRotation().x), glm::vec3(1.0f, 0.0f, 0.0f));
		m_vsData.modelMatrix = glm::rotate(m_vsData.modelMatrix, 
			glm::radians(Inputs::Instance().GetRotation().y), glm::vec3(0.0f, 1.0f, 0.0f));
		m_vsData.modelMatrix = glm::rotate(m_vsData.modelMatrix, 
			glm::radians(Inputs::Instance().GetRotation().z), glm::vec3(0.0f, 0.0f, 1.0f));

		UInt8 * data;
		VK_CHECK_RESULT(vkMapMemory(Device::Instance().GetLogicalDevice(), 
			m_vsBuffer.memory, 0, sizeof(m_vsData), 0, (void **)&data));
		memcpy(data, &m_vsData, sizeof(m_vsData));
		vkUnmapMemory(Device::Instance().GetLogicalDevice(), m_vsBuffer.memory);
	}
}