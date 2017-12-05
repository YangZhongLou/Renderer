/*
* This code is licensed under the MIT license (MIT) (http://opensource.org/licenses/MIT)
*/
#pragma once

namespace Concise
{
	namespace Utils
	{
		#define VK_CHECK_RESULT(func)																			\
		{																										\
			VkResult res = (func);																				\
			if (res != VK_SUCCESS)																				\
			{																									\
				std::cout << "Fatal : VkResult is \"" << vks::tools::errorString(res) << "\" in " << __FILE__ << " at line " << __LINE__ << std::endl; \
				assert(res == VK_SUCCESS);																		\
			}																									\
		}
	
		#define LOG_ERROR(str) std::cout << "Error! \"" << str << "\" in " << __FILE__ << " at line " << __LINE__ << std::endl;

	}
}
