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
		VkSurfaceKHR m_surface;
		
		UInt32 m_imageCount;
		std::vector<VkImage> m_images;
		std::vector<SwapchainBuffer> m_buffers;
		VkSwapchainKHR m_swapChain = VK_NULL_HANDLE;
		VkFormat m_colorFormat;
		VkColorSpaceKHR m_colorSpace;
		
		
#if defined(_WIN32)
		HWND m_window;
		HINSTANCE m_windowInstance;
#endif

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
		Swapchain(VkInstance instance, Device * device);
		~Swapchain();
	public:
		void Init();
		
		/** TODO RAII */
		void CreateSwapchain(UInt32 * width, UInt32 * height, bool vsync = false);
		HWND CreateWindow(HINSTANCE hinstance, WNDPROC wndproc);
		UInt32 GetImageCount() { return m_imageCount; }
		SwapchainBuffer GetBuffer(UInt32 i) { return m_buffers[i]; }
		VkFormat GetColorFormat() { return m_colorFormat; }
		
		VkResult AcquireNextImage();
		
#if defined(_WIN32)
		HWND GetWindow();
		HINSTANCE GetWindowInstance();
#endif
		
	private:
		/** function from dll */
		void InitDllFunction();
		void InitSurface();
		void InitWin32Surface();
	}
}