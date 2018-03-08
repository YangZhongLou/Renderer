#include "commandbuffers.h"
#include "device.h"
#include "vk_factory.hpp"
#include "defines.h"

namespace Concise
{
	CommandBuffers::CommandBuffers(UInt32 size)
	{
		VkCommandBufferAllocateInfo cmdBufAllocateInfo = 
			VkFactory::CommandBufferAllocateInfo(
				Device::Instance().GetCommandPool(), 
				VK_COMMAND_BUFFER_LEVEL_PRIMARY, 
				static_cast<UInt32>(m_drawCmdBuffers.size()));

		VK_CHECK_RESULT(vkAllocateCommandBuffers(
			LogicalDevice, 
			&cmdBufAllocateInfo, 
			m_drawCmdBuffers.data()));
	}

	CommandBuffers::~CommandBuffers()
	{

	}

}