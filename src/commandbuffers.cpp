#include "commandbuffers.h"

namespace Concise
{
	CommandBuffers::CommandBuffers()
	{
		m_drawCmdBuffers.resize(m_swapchain->GetImageCount());

		VkCommandBufferAllocateInfo cmdBufAllocateInfo = VkFactory::CommandBufferAllocateInfo(m_device->GetCommandPool(), VK_COMMAND_BUFFER_LEVEL_PRIMARY, static_cast<UInt32>(m_drawCmdBuffers.size()));

		VK_CHECK_RESULT(vkAllocateCommandBuffers(m_device->GetLogicalDevice(), &cmdBufAllocateInfo, m_drawCmdBuffers.data()));
	}
}