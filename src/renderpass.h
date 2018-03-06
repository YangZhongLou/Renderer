/**
* Author ZhongLou Yang, 2018-2-24
*/

#pragma once

#include <vulkan/vulkan.h>

namespace Concise
{
	class Device;

	class Renderpass
	{
	private:
		VkRenderPass m_renderpass;
		Device * m_device;
	public:
		Renderpass(VkFormat colorFormat, VkFormat depthFormat);
		~Renderpass();
	public:
		VkRenderPass & Get() { return m_renderpass; }
	};
}