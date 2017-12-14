/*
* This code is licensed under the MIT license (MIT) (http://opensource.org/licenses/MIT)
*/
#pragma once

#include <vulkan/vulkan.h>

namespace Concise
{
	class Debugger
	{
	private:
		PFN_vkCreateDebugReportCallbackEXT CreateDebugReportCallback = VK_NULL_HANDLE;
		PFN_vkDestroyDebugReportCallbackEXT DestroyDebugReportCallback = VK_NULL_HANDLE;
		PFN_vkDebugReportMessageEXT DebugReportMessage = VK_NULL_HANDLE;
	
		VkInstance m_vkInstance;
		VkDebugReportFlagsEXT m_flags;
		VkDebugReportCallbackEXT m_callback;
	public:
		Debugger(VkInstance instance, VkDebugReportFlagsEXT flags);
		~Debugger();
	public:
		void Init();
	}
}