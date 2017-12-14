/*
* This code is licensed under the MIT license (MIT) (http://opensource.org/licenses/MIT)
*/
#pragma once

#define DEFAULT_FENCE_TIMEOUT 100000000000

#define VK_CHECK_RESULT(func)																							\
{																														\
	VkResult res = (func);																								\
	if (res != VK_SUCCESS)																								\
	{																													\
		std::cout << "Fatal : VkResult is \"" << res << "\" in " << __FILE__ << " at line " << __LINE__ << std::endl; 	\
		assert(res == VK_SUCCESS);																						\
	}																													\
}

#define GET_INSTANCE_PROCEDURE_ADDR(inst, entrypoint)                  										\
	{                                                                      								 	\
		entrypoint = reinterpret_cast<PFN_vk##entrypoint>(vkGetInstanceProcAddr(inst, "vk"#entrypoint)); 	\
		if (entrypoint == NULL)                                         									\
		{																    								\
			exit(1);                                                        								\
		}                                                                   								\
	}

#define GET_DEVICE_PROCEDURE_ADDR(dev, entrypoint)                           							\
	{                                                                       							\
		entrypoint = reinterpret_cast<PFN_vk##entrypoint>(vkGetDeviceProcAddr(dev, "vk"#entrypoint));   \
		if (entrypoint == NULL)                                         								\
		{																   								\
			exit(1);                                                        							\
		}                                                                   							\
	}

#define LOG_ERROR(str) std::cout << "Error! \"" << str << "\" in " << __FILE__ << " at line " << __LINE__ << std::endl;
#define SAFE_DELETE(a) if(a) delete a;
#define SAFE_DELETE_N(a) if(a) delete [] a;