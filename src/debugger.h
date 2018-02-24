/**
* Author ZhongLou Yang, 2018-2-5
*/
#pragma once

#include "types.h"
#include <vulkan/vulkan.h>

namespace Concise
{
	class Debugger
	{
	public:
		static Int32 validationLayerCount;
		static const char * validationLayerNames[];
	private:
		PFN_vkCreateDebugReportCallbackEXT CreateDebugReportCallback = VK_NULL_HANDLE;
		PFN_vkDestroyDebugReportCallbackEXT DestroyDebugReportCallback = VK_NULL_HANDLE;
		PFN_vkDebugReportMessageEXT DebugReportMessage = VK_NULL_HANDLE;
	
		VkDebugReportFlagsEXT m_flags;
		VkDebugReportCallbackEXT m_callback;
	public:
		Debugger(VkDebugReportFlagsEXT flags);
		~Debugger();
	};
}