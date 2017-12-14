/*
* This code is licensed under the MIT license (MIT) (http://opensource.org/licenses/MIT)
*/

#include <Windows.h>
#include "utils.h"
#include <vulkan/vulkan.h>

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
	}
}