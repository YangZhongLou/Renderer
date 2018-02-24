#include "vk_instance.h"
#include "vk_factory.hpp"
#include "defines.h"
#include "debugger.h"

namespace Concise
{
	VulkanInstance::VulkanInstance()
	{
		VkApplicationInfo appInfo = VkFactory::ApplicationInfo();
		std::vector<const char*> instanceExtensions = { VK_KHR_SURFACE_EXTENSION_NAME };

	#if defined(_WIN32)
		instanceExtensions.push_back(VK_KHR_WIN32_SURFACE_EXTENSION_NAME);
	#elif defined(VK_USE_PLATFORM_ANDROID_KHR)
		instanceExtensions.push_back(VK_KHR_ANDROID_SURFACE_EXTENSION_NAME);
	#elif defined(_DIRECT2DISPLAY)
		instanceExtensions.push_back(VK_KHR_DISPLAY_EXTENSION_NAME);
	#elif defined(VK_USE_PLATFORM_WAYLAND_KHR)
		instanceExtensions.push_back(VK_KHR_WAYLAND_SURFACE_EXTENSION_NAME);
	#elif defined(VK_USE_PLATFORM_XCB_KHR)
		instanceExtensions.push_back(VK_KHR_XCB_SURFACE_EXTENSION_NAME);
	#elif defined(VK_USE_PLATFORM_IOS_MVK)
		instanceExtensions.push_back(VK_MVK_IOS_SURFACE_EXTENSION_NAME);
	#elif defined(VK_USE_PLATFORM_MACOS_MVK)
		instanceExtensions.push_back(VK_MVK_MACOS_SURFACE_EXTENSION_NAME);
	#endif

		VkInstanceCreateInfo instanceCreateInfo = VkFactory::InstanceCreateInfo(appInfo);
		if (instanceExtensions.size() > 0)
		{
	#if _DEBUG
			instanceExtensions.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
	#endif
			instanceCreateInfo.enabledExtensionCount = static_cast<UInt32>(instanceExtensions.size());
			instanceCreateInfo.ppEnabledExtensionNames = instanceExtensions.data();
		}
	#if _DEBUG
		instanceCreateInfo.enabledLayerCount = Debugger::validationLayerCount;
		instanceCreateInfo.ppEnabledLayerNames = Debugger::validationLayerNames;

		m_debugger = new Debugger(VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT);
	#endif
		VK_CHECK_RESULT(vkCreateInstance(&instanceCreateInfo, nullptr, &m_instance));
	}
	
	VulkanInstance::~VulkanInstance()
	{
		vkDestroyInstance(m_instance, nullptr);
		SAFE_DELETE(m_debugger);
	}
}