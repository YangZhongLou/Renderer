/*
* This code is licensed under the MIT license (MIT) (http://opensource.org/licenses/MIT)
*/
#pragma once

#include "types.h"
#include <vector>
#include <vulkan/vulkan.h>

namespace Concise
{
	class Device;
	
	struct Vertex
	{
		float positions[4];
		float colors[4];
	};
	
	struct Buffer
	{
		VkDeviceMemory memory;
		VkBuffer buffer;
	};
	
	class Vertices
	{
	private:
		Buffer m_vertexBuffer;
		Buffer m_indexBuffer;
		UInt32 m_indexCount;
		Device * m_device;
	public:
		Vertices(Device * device);
		~Vertices();
	public:
		void Submit(std::vector<Vertex>& vertexData, std::vector<UInt32>& indexData);
		const Buffer & GetVertexBuffer() const { return m_vertexBuffer; }
		const Buffer & GetIndexBuffer() const { return m_indexBuffer; }
		UInt32 GetIndexCount() const { return m_indexCount; };
	};
}