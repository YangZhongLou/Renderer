/**
* Author ZhongLou Yang, 2018-2-5
*/
#pragma once

#include "types.h"
#include <vector>
#include <vulkan/vulkan.h>

namespace Concise
{
	class Device;
	
	enum VertexLayoutComponent 
	{
		VERTEX_COMPONENT_POSITION = 0x0,
		VERTEX_COMPONENT_NORMAL = 0x1,
		VERTEX_COMPONENT_COLOR = 0x2,
		VERTEX_COMPONENT_UV = 0x3,
		VERTEX_COMPONENT_TANGENT = 0x4,
		VERTEX_COMPONENT_BITANGENT = 0x5,
		VERTEX_COMPONENT_DUMMY_FLOAT = 0x6,
		VERTEX_COMPONENT_DUMMY_VEC4 = 0x7
	};
	
	struct VertexLayout
	{
		std::vector<VertexLayoutComponent> components;

		VertexLayout(std::vector<VertexLayoutComponent> components)
		{
			this->components = std::move(components);
		}

		UInt32 Stride()
		{
			UInt32 sum = 0;
			for (auto & component : components)
			{
				switch (component)
				{
				case VERTEX_COMPONENT_UV:
					sum += 2 * sizeof(float);
					break;
				case VERTEX_COMPONENT_DUMMY_FLOAT:
					sum += sizeof(float);
					break;
				case VERTEX_COMPONENT_DUMMY_VEC4:
					sum += 4 * sizeof(float);
					break;
				default:
					// All components except the ones listed above are made up of 3 floats
					sum += 3 * sizeof(float);
				}
			}
			return sum;
		}
	}
	
	/** TODO */
	struct Vertex
	{
		float positions[3];
		float colors[3];
	};

	struct Buffer;
	
	#define DEFAULT_VERTEX_DATA_SIZE 2048 * sizeof(float) * 16
	#define DEFAULT_INDEX_DATA_SIZE 2048 * sizeof(UInt32) * 16
	#define DEFAULT_STAGING_BUFFER_SIZE 2048 * sizeof(UInt32)
	
	class Vertices
	{
	private:
		Buffer * m_vertexStagingBuffer;
		Buffer * m_vertexBuffer;
		UInt32 m_vertexDataOffset;
		
		Buffer * m_indexStagingBuffer;
		Buffer * m_indexBuffer;
		UInt32 m_indexDataOffset;
		UInt32 m_indexCount;
		
		Device * m_device;
	public:
		Vertices(Device * device);
		~Vertices();
	public:
		void Init();
		void Submit(std::vector<Vertex>& vertexData, std::vector<UInt32>& indexData);
		const Buffer & GetVertexBuffer() const { return *m_vertexBuffer; }
		const Buffer & GetIndexBuffer() const { return *m_indexBuffer; }
		UInt32 GetIndexCount() const { return m_indexCount; };
	};
}