/*
* This code is licensed under the MIT license (MIT) (http://opensource.org/licenses/MIT)
*/

#pragma once

#include <vulkan/vulkan.h>


namespace Concise
{
	class IndexBuffer
	{
	public:
		VkDeviceMemory memory;		
		VkBuffer buffer;			
		uint32_t count;
	};
}
