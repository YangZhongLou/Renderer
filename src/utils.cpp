/*
* This code is licensed under the MIT license (MIT) (http://opensource.org/licenses/MIT)
*/

#include "utils.h"

namespace Concise
{
	namespace Utils
	{
		void ExitFatal(std::string message, std::string caption, bool silent)
		{
#if defined(_WIN32)
			if (!silent) 
			{
				MessageBox(NULL, message.c_str(), caption.c_str(), MB_OK | MB_ICONERROR);
			}
#elif defined(__ANDROID__)	
			LOGE("Fatal error: %s", message.c_str());
#endif
			std::cerr << message << "\n";
			exit(1);
		}
		
		void SetupDebugging(VkInstance instance, VkDebugReportFlagsEXT flags, VkDebugReportCallbackEXT callBack)
		{
			CreateDebugReportCallback = reinterpret_cast<PFN_vkCreateDebugReportCallbackEXT>(vkGetInstanceProcAddr(instance, "vkCreateDebugReportCallbackEXT"));
			DestroyDebugReportCallback = reinterpret_cast<PFN_vkDestroyDebugReportCallbackEXT>(vkGetInstanceProcAddr(instance, "vkDestroyDebugReportCallbackEXT"));
			dbgBreakCallback = reinterpret_cast<PFN_vkDebugReportMessageEXT>(vkGetInstanceProcAddr(instance, "vkDebugReportMessageEXT"));

			VkDebugReportCallbackCreateInfoEXT dbgCreateInfo = {};
			dbgCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CREATE_INFO_EXT;
			dbgCreateInfo.pfnCallback = (PFN_vkDebugReportCallbackEXT)messageCallback;
			dbgCreateInfo.flags = flags;

			VkResult err = CreateDebugReportCallback(
				instance,
				&dbgCreateInfo,
				nullptr,
				(callBack != VK_NULL_HANDLE) ? &callBack : &msgCallback);
			assert(!err);
		}
	}
}