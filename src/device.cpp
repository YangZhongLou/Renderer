/*
* This code is licensed under the MIT license (MIT) (http://opensource.org/licenses/MIT)
*/

#include "device.h"

namespace Concise
{
	Device::Device()
	{
		
	}
	
	Device::~Device()
	{
		
	}
	
	UInt32 Device::GetMemoryTypeIndex(UInt32 typeBits, VkMemoryPropertyFlags properties)
	{
		for (uint32_t i = 0; i < m_deviceMemoryProperties.memoryTypeCount; i++)
		{
			if ((typeBits & 1) == 1)
			{
				if ((m_deviceMemoryProperties.memoryTypes[i].propertyFlags & properties) == properties)
				{						
					return i;
				}
			}
			typeBits >>= 1;
		}
		
		LOG_ERROR("GetMemoryTypeIndex");
	}
}