/**
* Author ZhongLou Yang, 2018-2-5
*/
#pragma once

#include <vector>
#include <vulkan/vulkan.h>
#include "types.h"


namespace Concise
{
	class Drawboard;
	
	struct SwapchainBuffer
	{
		VkImage image;
		VkImageView view;
	};
	
	class Swapchain
	{
	private:
		Drawboard * m_drawboard;

		VkSurfaceKHR m_surface;
		
		UInt32 m_imageCount;
		std::vector<VkImage> m_images;
		std::vector<SwapchainBuffer> m_buffers;
		VkSwapchainKHR m_swapchain = VK_NULL_HANDLE;
		VkFormat m_colorFormat;
		VkColorSpaceKHR m_colorSpace;

		UInt32 m_queueNodeIndex = UINT32_MAX;

	private:
		PFN_vkGetPhysicalDeviceSurfaceSupportKHR GetPhysicalDeviceSurfaceSupportKHR;
		PFN_vkGetPhysicalDeviceSurfaceCapabilitiesKHR GetPhysicalDeviceSurfaceCapabilitiesKHR; 
		PFN_vkGetPhysicalDeviceSurfaceFormatsKHR GetPhysicalDeviceSurfaceFormatsKHR;
		PFN_vkGetPhysicalDeviceSurfacePresentModesKHR GetPhysicalDeviceSurfacePresentModesKHR;
		PFN_vkCreateSwapchainKHR CreateSwapchainKHR;
		PFN_vkDestroySwapchainKHR DestroySwapchainKHR;
		PFN_vkGetSwapchainImagesKHR GetSwapchainImagesKHR;
		PFN_vkAcquireNextImageKHR AcquireNextImageKHR;
		PFN_vkQueuePresentKHR QueuePresentKHR;
		
	public:
		Swapchain(Drawboard * drawboard);
		~Swapchain();
	public:		
		void CreateSwapchain(UInt32 * width, UInt32 * height, bool vsync = false);
		UInt32 GetImageCount() { return m_imageCount; }
		SwapchainBuffer & GetBuffer(UInt32 i) { return m_buffers[i]; }
		VkFormat GetColorFormat() const { return m_colorFormat; }
		
		VkResult AcquireNextImage(VkSemaphore presentCompleteSemaphore, UInt32 *imageIndex);
		VkResult QueuePresent(VkQueue queue, UInt32 imageIndex, VkSemaphore waitSemaphore = VK_NULL_HANDLE);
	private:
		/** function from dll */
		void LoadDllFunction();
		void InitSurface();
		void InitWin32Surface();
	};
}