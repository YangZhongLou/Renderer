/*
* This code is licensed under the MIT license (MIT) (http://opensource.org/licenses/MIT)
*/
#pragma once

#include <vector>
#include <vulkan/vulkan.h>
#include "vertex.hpp"

namespace Concise
{
	class Device;
	
	struct Buffer
	{
		VkDeviceMemory m_memory;
		VkBuffer m_buffer;
	}
	
	class Vertices
	{
	private:
		Buffer m_vertexBuffer;
		std::vector<Vertex> m_data;
	public:
		Vertices(Device * device);
	}
}