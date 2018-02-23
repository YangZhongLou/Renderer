/**
* Author ZhongLou Yang, 2018-2-5
*/

#pragma once

#include <vulkan/vulkan.h>

namespace Concise
{
	class Device;
	
	class Descriptors;
	
	class Pipeline
	{
	private:
		Device * m_device;

	public:
		Pipeline(Device * device);
		~Pipeline();
	};
}