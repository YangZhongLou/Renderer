#include "debugger.h"
#include "utils.h"
#include "vkfactory.hpp"
#include <string>

namespace Concise
{
	Debugger::Debugger(VkInstance instance) : m_vkInstance(instance)
	{
	}
	
	Debugger::~Debugger()
	{
		
	}
	
	VKAPI_ATTR VkBool32 VKAPI_CALL messageCallback(
		VkDebugReportFlagsEXT flags,
		VkDebugReportObjectTypeEXT objType,
		uint64_t srcObject,
		size_t location,
		int32_t msgCode,
		const char* pLayerPrefix,
		const char* pMsg,
		void* pUserData)
	{
		std::string prefix("");

		// Error that may result in undefined behaviour
		if (flags & VK_DEBUG_REPORT_ERROR_BIT_EXT)
		{
			prefix += "ERROR:";
		};
		// Warnings may hint at unexpected / non-spec API usage
		if (flags & VK_DEBUG_REPORT_WARNING_BIT_EXT)
		{
			prefix += "WARNING:";
		};
		// May indicate sub-optimal usage of the API
		if (flags & VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT)
		{
			prefix += "PERFORMANCE:";
		};
		// Informal messages that may become handy during debugging
		if (flags & VK_DEBUG_REPORT_INFORMATION_BIT_EXT)
		{
			prefix += "INFO:";
		}
		// Diagnostic info from the Vulkan loader and layers
		// Usually not helpful in terms of API usage, but may help to debug layer and loader problems 
		if (flags & VK_DEBUG_REPORT_DEBUG_BIT_EXT)
		{
			prefix += "DEBUG:";
		}

		// Display message to default output (console/logcat)
		std::stringstream debugMessage;
		debugMessage << prefix << " [" << pLayerPrefix << "] Code " << msgCode << " : " << pMsg;

#if defined(__ANDROID__)
		if (flags & VK_DEBUG_REPORT_ERROR_BIT_EXT) {
			LOGE("%s", debugMessage.str().c_str());
		}
		else {
			LOGD("%s", debugMessage.str().c_str());
		}
#else
		if (flags & VK_DEBUG_REPORT_ERROR_BIT_EXT) {
			std::cerr << debugMessage.str() << "\n";
		}
		else {
			std::cout << debugMessage.str() << "\n";
		}
#endif

		fflush(stdout);

		// The return value of this callback controls wether the Vulkan call that caused
		// the validation message will be aborted or not
		// We return VK_FALSE as we DON'T want Vulkan calls that cause a validation message 
		// (and return a VkResult) to abort
		// If you instead want to have calls abort, pass in VK_TRUE and the function will 
		// return VK_ERROR_VALIDATION_FAILED_EXT 
		return VK_FALSE;
	}
	
	void Debugger::Init()
	{
		CreateDebugReportCallback = reinterpret_cast<PFN_vkCreateDebugReportCallbackEXT>(vkGetInstanceProcAddr(m_vkInstance, "vkCreateDebugReportCallbackEXT"));
		DestroyDebugReportCallback = reinterpret_cast<PFN_vkDestroyDebugReportCallbackEXT>(vkGetInstanceProcAddr(m_vkInstance, "vkDestroyDebugReportCallbackEXT"));
		dbgBreakCallback = reinterpret_cast<PFN_vkDebugReportMessageEXT>(vkGetInstanceProcAddr(m_vkInstance, "vkDebugReportMessageEXT"));

		VkDebugReportCallbackCreateInfoEXT dbgCreateInfo = VkFactory::DebugReportCallbackCreateInfo();
		dbgCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CREATE_INFO_EXT;
		dbgCreateInfo.pfnCallback = (PFN_vkDebugReportCallbackEXT)messageCallback;
		dbgCreateInfo.flags = flags;

		Utils::VK_CHECK_RESULT(CreateDebugReportCallback(
			m_vkInstance,
			&dbgCreateInfo,
			nullptr,
			(callBack != VK_NULL_HANDLE) ? &callBack : &msgCallback));

	}
}