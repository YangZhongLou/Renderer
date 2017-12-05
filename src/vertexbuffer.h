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
	
	class VertexBuffer
	{
	private:
		Device * m_device;
		VkDeviceMemory m_memory;
		VkBuffer m_buffer;
		std::vector<Vertex> m_vertices;
	public:
		VertexBuffer(Device * device);
		~VertexBuffer();
	public:
		void Submit(const std::vector<Vertex>& vertices);
	}
}