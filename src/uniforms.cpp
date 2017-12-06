
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
		
	}
}