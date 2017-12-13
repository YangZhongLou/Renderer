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
		VkDebugReportCallbackEXT DebugReportCallback;
	
		VkInstance m_vkInstance;
	public:
		Debugger(VkInstance instance);
		~Debugger();
	public:
		void Init();
	}
}