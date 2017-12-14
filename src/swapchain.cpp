#include "swapchain.h"
#include "vkfactory.hpp"

namespace Concise
{
	Swapchain::Swapchain(VkInstance instance, Device * device) : m_instance(instance), m_device(device)
	{
	}
	
	Swapchain::~Swapchain()
	{
	}
	
	void Swapchain::Init()
	{
		InitDllFunction();
		InitSurface();
	}
	
	void Swapchain::InitDllFunction()
	{
		GET_INSTANCE_PROCEDURE_ADDR(m_instance, GetPhysicalDeviceSurfaceSupportKHR);
		GET_INSTANCE_PROCEDURE_ADDR(m_instance, GetPhysicalDeviceSurfaceCapabilitiesKHR);
		GET_INSTANCE_PROCEDURE_ADDR(m_instance, GetPhysicalDeviceSurfaceFormatsKHR);
		GET_INSTANCE_PROCEDURE_ADDR(m_instance, GetPhysicalDeviceSurfacePresentModesKHR);
		GET_DEVICE_PROCEDURE_ADDR(m_device->GetLogicalDevice(), CreateSwapchainKHR);
		GET_DEVICE_PROCEDURE_ADDR(m_device->GetLogicalDevice(), DestroySwapchainKHR);
		GET_DEVICE_PROCEDURE_ADDR(m_device->GetLogicalDevice(), GetSwapchainImagesKHR);
		GET_DEVICE_PROCEDURE_ADDR(m_device->GetLogicalDevice(), AcquireNextImageKHR);
		GET_DEVICE_PROCEDURE_ADDR(m_device->GetLogicalDevice(), QueuePresentKHR);
	}
	
	void Swapchain::InitSurface()
	{
#if defined(_WIN32)
		InitWin32Surface();
#endif
	}
	
	void Swapchain::InitWin32Surface()
	{
		VkWin32SurfaceCreateInfoKHR surfaceCreateInfo = VkFactory::Win32SurfaceCreateInfoKHR();
		surfaceCreateInfo.hinstance = (HINSTANCE)m_windowInstance;
		surfaceCreateInfo.hwnd = (HWND)m_window;
		Utils::VK_CHECK_RESULT(vkCreateWin32SurfaceKHR(m_instance, &surfaceCreateInfo, nullptr, &m_surface));
		
		UInt32 queueCount;
		vkGetPhysicalDeviceQueueFamilyProperties(m_device->GetPhysicalDevice(), &queueCount, NULL);
		assert(queueCount >= 1);

		std::vector<VkQueueFamilyProperties> queueProps(queueCount);
		vkGetPhysicalDeviceQueueFamilyProperties(m_device->GetPhysicalDevice(), &queueCount, queueProps.data());

		std::vector<VkBool32> supportsPresent(queueCount);
		for (UInt32 i = 0; i < queueCount; i++) 
		{
			GetPhysicalDeviceSurfaceSupportKHR(m_device->GetPhysicalDevice(), i, m_surface, &supportsPresent[i]);
		}

		UInt32 graphicsQueueNodeIndex = UINT32_MAX;
		UInt32 presentQueueNodeIndex = UINT32_MAX;
		for (UInt32 i = 0; i < queueCount; i++) 
		{
			if ((queueProps[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) != 0) 
			{
				if (graphicsQueueNodeIndex == UINT32_MAX) 
				{
					graphicsQueueNodeIndex = i;
				}

				if (supportsPresent[i] == VK_TRUE) 
				{
					graphicsQueueNodeIndex = i;
					presentQueueNodeIndex = i;
					break;
				}
			}
		}
		if (presentQueueNodeIndex == UINT32_MAX) 
		{	
			for (UInt32 i = 0; i < queueCount; ++i) 
			{
				if (supportsPresent[i] == VK_TRUE) 
				{
					presentQueueNodeIndex = i;
					break;
				}
			}
		}

		// Exit if either a graphics or a presenting queue hasn't been found
		if (graphicsQueueNodeIndex == UINT32_MAX || presentQueueNodeIndex == UINT32_MAX) 
		{
			Utils::ExitFatal("Could not find a graphics and/or presenting queue!", "Fatal error");
		}

		// todo : Add support for separate graphics and presenting queue
		if (graphicsQueueNodeIndex != presentQueueNodeIndex) 
		{			
			Utils::ExitFatal("Separate graphics and presenting queues are not supported yet!", "Fatal error");
		}

		queueNodeIndex = graphicsQueueNodeIndex;

		UInt32 formatCount;
		Utils::VK_CHECK_RESULT(fpGetPhysicalDeviceSurfaceFormatsKHR(m_device->GetPhysicalDevice(), m_surface, &formatCount, NULL));
		assert(formatCount > 0);

		std::vector<VkSurfaceFormatKHR> surfaceFormats(formatCount);
		Utils::VK_CHECK_RESULT(fpGetPhysicalDeviceSurfaceFormatsKHR(m_device->GetPhysicalDevice(), m_surface, &formatCount, surfaceFormats.data()));

		if ((formatCount == 1) && (surfaceFormats[0].format == VK_FORMAT_UNDEFINED))
		{
			m_colorFormat = VK_FORMAT_B8G8R8A8_UNORM;
			m_colorSpace = surfaceFormats[0].colorSpace;
		}
		else
		{
			bool found_B8G8R8A8_UNORM = false;
			for (auto&& surfaceFormat : surfaceFormats)
			{
				if (surfaceFormat.format == VK_FORMAT_B8G8R8A8_UNORM)
				{
					m_colorFormat = surfaceFormat.format;
					m_colorSpace = surfaceFormat.colorSpace;
					found_B8G8R8A8_UNORM = true;
					break;
				}
			}
			if (!found_B8G8R8A8_UNORM)
			{
				m_colorFormat = surfaceFormats[0].format;
				m_colorSpace = surfaceFormats[0].colorSpace;
			}
		}
	}
	
	HWND Swapchain::CreateWindow(HINSTANCE hinstance, WNDPROC wndproc);
	{
		m_windowInstance = hinstance;

		WNDCLASSEX wndClass;

		wndClass.cbSize = sizeof(WNDCLASSEX);
		wndClass.style = CS_HREDRAW | CS_VREDRAW;
		wndClass.lpfnWndProc = wndproc;
		wndClass.cbClsExtra = 0;
		wndClass.cbWndExtra = 0;
		wndClass.hInstance = hinstance;
		wndClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
		wndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
		wndClass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
		wndClass.lpszMenuName = NULL;
		wndClass.lpszClassName = name.c_str();
		wndClass.hIconSm = LoadIcon(NULL, IDI_WINLOGO);

		if (!RegisterClassEx(&wndClass))
		{
			std::cout << "Could not register window class!\n";
			fflush(stdout);
			exit(1);
		}

		int screenWidth = GetSystemMetrics(SM_CXSCREEN);
		int screenHeight = GetSystemMetrics(SM_CYSCREEN);

		if (m_settings.fullscreen)
		{
			DEVMODE dmScreenSettings;
			memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));
			dmScreenSettings.dmSize = sizeof(dmScreenSettings);
			dmScreenSettings.dmPelsWidth = screenWidth;
			dmScreenSettings.dmPelsHeight = screenHeight;
			dmScreenSettings.dmBitsPerPel = 32;
			dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

			if ((m_width != (UInt32)screenWidth) && (m_height != (UInt32)screenHeight))
			{
				if (ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN) != DISP_CHANGE_SUCCESSFUL)
				{
					if (MessageBox(NULL, "Fullscreen Mode not supported!\n Switch to window mode?", "Error", MB_YESNO | MB_ICONEXCLAMATION) == IDYES)
					{
						m_settings.fullscreen = false;
					}
					else
					{
						return nullptr;
					}
				}
			}

		}

		DWORD dwExStyle;
		DWORD dwStyle;

		if (m_settings.fullscreen)
		{
			dwExStyle = WS_EX_APPWINDOW;
			dwStyle = WS_POPUP | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;
		}
		else
		{
			dwExStyle = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;
			dwStyle = WS_OVERLAPPEDWINDOW | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;
		}

		RECT windowRect;
		windowRect.left = 0L;
		windowRect.top = 0L;
		windowRect.right = m_settings.fullscreen ? (long)screenWidth : (long)width;
		windowRect.bottom = m_settings.fullscreen ? (long)screenHeight : (long)height;

		AdjustWindowRectEx(&windowRect, dwStyle, FALSE, dwExStyle);

		std::string windowTitle = getWindowTitle();
		m_window = CreateWindowEx(0,
			name.c_str(),
			windowTitle.c_str(),
			dwStyle | WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
			0,
			0,
			windowRect.right - windowRect.left,
			windowRect.bottom - windowRect.top,
			NULL,
			NULL,
			hinstance,
			NULL);

		if (!m_settings.fullscreen)
		{
			// Center on screen
			UInt32 x = (GetSystemMetrics(SM_CXSCREEN) - windowRect.right) / 2;
			UInt32 y = (GetSystemMetrics(SM_CYSCREEN) - windowRect.bottom) / 2;
			SetWindowPos(m_window, 0, x, y, 0, 0, SWP_NOZORDER | SWP_NOSIZE);
		}

		if (!m_window)
		{
			printf("Could not create window!\n");
			fflush(stdout);
			return nullptr;
			exit(1);
		}

		ShowWindow(m_window, SW_SHOW);
		SetForegroundWindow(m_window);
		SetFocus(m_window);

		return m_window;
	}
	
	void Swapchain::CreateSwapchain(UInt32 * width, UInt32 * height, bool vsync = false)
	{
		VkSurfaceCapabilitiesKHR surfCaps;
		Utils::VK_CHECK_RESULT(GetPhysicalDeviceSurfaceCapabilitiesKHR(m_device->GetPhysicalDevice(), m_surface, &surfCaps));

		UInt32 presentModeCount;
		Utils::VK_CHECK_RESULT(fpGetPhysicalDeviceSurfacePresentModesKHR(m_device->GetPhysicalDevice(), m_surface, &presentModeCount, NULL));
		assert(presentModeCount > 0);

		std::vector<VkPresentModeKHR> presentModes(presentModeCount);
		Utils::VK_CHECK_RESULT(fpGetPhysicalDeviceSurfacePresentModesKHR(m_device->GetPhysicalDevice(), m_surface, &presentModeCount, presentModes.data()));

		VkExtent2D swapchainExtent = {};
		if (surfCaps.currentExtent.width == (UInt32)-1)
		{
			swapchainExtent.width = *width;
			swapchainExtent.height = *height;
		}
		else
		{
			swapchainExtent = surfCaps.currentExtent;
			*width = surfCaps.currentExtent.width;
			*height = surfCaps.currentExtent.height;
		}

		VkPresentModeKHR swapchainPresentMode = VK_PRESENT_MODE_FIFO_KHR;
		if (!vsync)
		{
			for (size_t i = 0; i < presentModeCount; i++)
			{
				if (presentModes[i] == VK_PRESENT_MODE_MAILBOX_KHR)
				{
					swapchainPresentMode = VK_PRESENT_MODE_MAILBOX_KHR;
					break;
				}
				if ((swapchainPresentMode != VK_PRESENT_MODE_MAILBOX_KHR) && (presentModes[i] == VK_PRESENT_MODE_IMMEDIATE_KHR))
				{
					swapchainPresentMode = VK_PRESENT_MODE_IMMEDIATE_KHR;
				}
			}
		}

		UInt32 desiredNumberOfSwapchainImages = surfCaps.minImageCount + 1;
		if ((surfCaps.maxImageCount > 0) && (desiredNumberOfSwapchainImages > surfCaps.maxImageCount))
		{
			desiredNumberOfSwapchainImages = surfCaps.maxImageCount;
		}

		VkSurfaceTransformFlagsKHR preTransform;
		if (surfCaps.supportedTransforms & VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR)
		{
			preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
		}
		else
		{
			preTransform = surfCaps.currentTransform;
		}

		VkCompositeAlphaFlagBitsKHR compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
		std::vector<VkCompositeAlphaFlagBitsKHR> compositeAlphaFlags = 
		{
			VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
			VK_COMPOSITE_ALPHA_PRE_MULTIPLIED_BIT_KHR,
			VK_COMPOSITE_ALPHA_POST_MULTIPLIED_BIT_KHR,
			VK_COMPOSITE_ALPHA_INHERIT_BIT_KHR,
		};
		for (auto& compositeAlphaFlag : compositeAlphaFlags) 
		{
			if (surfCaps.supportedCompositeAlpha & compositeAlphaFlag) 
			{
				compositeAlpha = compositeAlphaFlag;
				break;
			};
		}

		VkSwapchainKHR oldSwapchain = m_swapchain;
		VkSwapchainCreateInfoKHR swapchainCreateInfo = VkFactory::SwapchainCreateInfoKHR();
		swapchainCreateInfo.surface = m_surface;
		swapchainCreateInfo.minImageCount = desiredNumberOfSwapchainImages;
		swapchainCreateInfo.imageFormat = m_colorFormat;
		swapchainCreateInfo.imageColorSpace = m_colorSpace;
		swapchainCreateInfo.imageExtent = { swapchainExtent.width, swapchainExtent.height };
		swapchainCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
		swapchainCreateInfo.preTransform = (VkSurfaceTransformFlagBitsKHR)preTransform;
		swapchainCreateInfo.imageArrayLayers = 1;
		swapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		swapchainCreateInfo.queueFamilyIndexCount = 0;
		swapchainCreateInfo.pQueueFamilyIndices = NULL;
		swapchainCreateInfo.presentMode = swapchainPresentMode;
		swapchainCreateInfo.oldSwapchain = oldSwapchain;
		swapchainCreateInfo.clipped = VK_TRUE;
		swapchainCreateInfo.compositeAlpha = compositeAlpha;

		VkFormatProperties formatProps;
		vkGetPhysicalDeviceFormatProperties(m_device->GetPhysicalDevice(), colorFormat, &formatProps);
		if ((formatProps.optimalTilingFeatures & VK_FORMAT_FEATURE_TRANSFER_SRC_BIT_KHR) || (formatProps.optimalTilingFeatures & VK_FORMAT_FEATURE_BLIT_SRC_BIT)) 
		{
			swapchainCreateInfo.imageUsage |= VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
		}

		Utils::VK_CHECK_RESULT(fpCreateSwapchainKHR(m_device->GetLogicalDevice(), &swapchainCreateInfo, nullptr, &m_swapchain));

		if (oldSwapchain != VK_NULL_HANDLE) 
		{ 
			for (UInt32 i = 0; i < m_imageCount; i++)
			{
				vkDestroyImageView(m_device->GetLogicalDevice(), m_buffers[i].view, nullptr);
			}
			fpDestroySwapchainKHR(m_device->GetLogicalDevice(), oldSwapchain, nullptr);
		}
		Utils::VK_CHECK_RESULT(fpGetSwapchainImagesKHR(m_device->GetLogicalDevice(), m_swapchain, &m_imageCount, NULL));

		m_images.resize(m_imageCount);
		Utils::VK_CHECK_RESULT(fpGetSwapchainImagesKHR(m_device->GetLogicalDevice(), m_swapchain, &m_imageCount, m_images.data()));

		m_buffers.resize(m_imageCount);
		for (UInt32 i = 0; i < m_imageCount; i++)
		{
			VkImageSubresourceRange subresourceRange;
			subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			subresourceRange.baseMipLevel = 0;
			subresourceRange.levelCount = 1;
			subresourceRange.baseArrayLayer = 0;
			subresourceRange.layerCount = 1;
			
			VkImageViewCreateInfo colorAttachmentView = VkFactory::ImageViewCreateInfo(colorFormat, m_images[i], subresourceRange);
			colorAttachmentView.components = 
			{
				VK_COMPONENT_SWIZZLE_R,
				VK_COMPONENT_SWIZZLE_G,
				VK_COMPONENT_SWIZZLE_B,
				VK_COMPONENT_SWIZZLE_A
			};
			colorAttachmentView.viewType = VK_IMAGE_VIEW_TYPE_2D;
			colorAttachmentView.flags = 0;
			m_buffers[i].image = m_images[i];

			Utils:;VK_CHECK_RESULT(vkCreateImageView(m_device->GetLogicalDevice(), &colorAttachmentView, nullptr, &m_buffers[i].view));
		}
	}
	
	HWND Swapchain::GetWindow()
	{
		return m_window;
	}
	
	HINSTANCE Swapchain::GetWindowInstance()
	{
		return m_windowInstance;
	}
	
	VkResult Swapchain::AcquireNextImage(VkSemaphore presentCompleteSemaphore, UInt32 * imageIndex)
	{
		return AcquireNextImageKHR(m_device->GetLogicalDevice(), m_swapchain, UINT64_MAX, presentCompleteSemaphore, (VkFence)nullptr, imageIndex);
	}
	
	VkResult Swapchain::QueuePresent(VkQueue queue, UInt32 imageIndex, VkSemaphore waitSemaphore = VK_NULL_HANDL)
	{
		VkPresentInfoKHR presentInfo = VkFactory::PresentInfoKHR(&m_swapchain, imageIndex);
		if (waitSemaphore != VK_NULL_HANDLE)
		{
			presentInfo.pWaitSemaphores = &waitSemaphore;
			presentInfo.waitSemaphoreCount = 1;
		}
		
		return QueuePresentKHR(queue, &presentInfo);
	}
}