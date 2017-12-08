/*
* This code is licensed under the MIT license (MIT) (http://opensource.org/licenses/MIT)
*/
#pragma once

#include <vulkan/vulkan.h>

namespace Concise
{
	class Device;
	
	struct SwapchainBuffer
	{
		VkImage image;
		VkImageView view;
	};
	
	class Swapchain
	{
	private:
		VkInstance m_instance;
		Device * m_device;
		std::vector<SwapchainBuffer> m_buffers;

		VkFormat m_colorFormat;
		
		UInt32 m_imageCount;
	public:
		Swapchain(VkInstance instance, Device * device);
		~Swapchain();
	public:
		void Init();
		UInt32 GetImageCount() { return m_imageCount; }
		SwapchainBuffer GetBuffer(UInt32 i) { return m_buffers[i]; }
		VkFormat GetColorFormat() { return m_colorFormat; }
		
	private:
		/** function from dll */
		void InitDllFunction();
		void InitSurface();
	}
}