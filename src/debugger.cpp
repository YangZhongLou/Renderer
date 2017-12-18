#include "debugger.h"
#include "utils.h"
#include "vkfactory.hpp"
#include <string>
#include <sstream>
#include <iostream>
#include <fstream>

namespace Concise
{
	Int32 Debugger::validationLayerCount = 1;
	const char * Debugger::validationLayerNames[] =
	{
		"VK_LAYER_LUNARG_standard_validation",
	};

	Debugger::Debugger(VkInstance instance, VkDebugReportFlagsEXT flags) : m_vkInstance(instance), m_flags(flags)
	{
	}

	Debugger::~Debugger()
	{
		if (m_callback != VK_NULL_HANDLE)
		{
			DestroyDebugReportCallback(m_vkInstance, m_callback, nullptr);
		}
	}

	void Dump(std::string & message)
	{
		std::ofstream fout("DebugInfo.txt", std::ios::out | std::ios::ate);

		if (fout.is_open())
		{
			fout << message;
			fout.close();
		}
	}

	VKAPI_ATTR VkBool32 VKAPI_CALL DebugReportCallback(
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

		if (flags & VK_DEBUG_REPORT_ERROR_BIT_EXT)
		{
			prefix += "ERROR:";
		};

		if (flags & VK_DEBUG_REPORT_WARNING_BIT_EXT)
		{
			prefix += "WARNING:";
		};

		if (flags & VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT)
		{
			prefix += "PERFORMANCE:";
		};

		if (flags & VK_DEBUG_REPORT_INFORMATION_BIT_EXT)
		{
			prefix += "INFO:";
		}
		if (flags & VK_DEBUG_REPORT_DEBUG_BIT_EXT)
		{
			prefix += "DEBUG:";
		}

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
		if (flags & VK_DEBUG_REPORT_ERROR_BIT_EXT)
		{
			std::cerr << debugMessage.str() << "\n";
			Dump(debugMessage.str());
		}
		else
		{
			std::cout << debugMessage.str() << "\n";
		}
#endif

		fflush(stdout);

		return VK_FALSE;
	}

	void Debugger::Init()
	{
		CreateDebugReportCallback = reinterpret_cast<PFN_vkCreateDebugReportCallbackEXT>(vkGetInstanceProcAddr(m_vkInstance, "vkCreateDebugReportCallbackEXT"));
		DestroyDebugReportCallback = reinterpret_cast<PFN_vkDestroyDebugReportCallbackEXT>(vkGetInstanceProcAddr(m_vkInstance, "vkDestroyDebugReportCallbackEXT"));
		DebugReportMessage = reinterpret_cast<PFN_vkDebugReportMessageEXT>(vkGetInstanceProcAddr(m_vkInstance, "vkDebugReportMessageEXT"));

		VkDebugReportCallbackCreateInfoEXT dbgCreateInfo = VkFactory::DebugReportCallbackCreateInfo(DebugReportCallback, m_flags);
		VK_CHECK_RESULT(CreateDebugReportCallback(
			m_vkInstance,
			&dbgCreateInfo,
			nullptr,
			&m_callback));
	}
}