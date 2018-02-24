#include "framebuffers.h"
#include "swapchain.h"
#include "device.h"
#include "defines.h"
#include "vk_factory.hpp"

namespace Concise
{
	Framebuffers::Framebuffers(Swapchain * swapchain, 
		VkRenderPass renderpass, 
		UInt32 width, UInt32 height)
	{
		m_framebuffers.resize(swapchain->GetImageCount());
		for (UInt32 i = 0; i < static_cast<UInt32>(m_framebuffers.size()); i++)
		{
			std::vector<VkImageView> attachments(2);
			attachments[0] = swapchain->GetBuffer(i).view;
			attachments[1] = Device::Instance().GetDepthStencil().view;

			VkFramebufferCreateInfo frameBufferCreateInfo =
				VkFactory::FramebufferCreateInfo(renderpass, attachments, width, height);
			VK_CHECK_RESULT(vkCreateFramebuffer(Device::Instance().GetLogicalDevice(),
				&frameBufferCreateInfo, nullptr, &m_framebuffers[i]));
		}
	}

	Framebuffers::~Framebuffers()
	{
		for (UInt32 i = 0; i < m_framebuffers.size(); i++)
		{
			vkDestroyFramebuffer(Device::Instance().GetLogicalDevice(), m_framebuffers[i], nullptr);
		}
	}
}