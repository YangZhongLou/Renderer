/**
* Author ZhongLou Yang, 2018-2-5
*/
#pragma once

#include <vector>
#include <vulkan/vulkan.h>
#include "types.h"

namespace Concise
{	
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
	};
	
	/** TODO */
	struct Vertex
	{
		float positions[3];
		float colors[3];
	};

	struct VerticesInput
	{
		VkPipelineVertexInputStateCreateInfo inputState;
		std::vector<VkVertexInputBindingDescription> bindingDescriptions;
		std::vector<VkVertexInputAttributeDescription> attributeDescriptions;
	};

	struct Buffer;
	
	#define VERTEX_BUFFER_BIND_ID 0

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

		VerticesInput m_verticesInput;
		
	public:
		static Vertices & Instance() 
		{
			static Vertices instance;
			return instance;
		}
	private:
		Vertices();
		~Vertices();
	public:
		void SetInputLayout(VertexLayout & vertexLayout);
		void Submit(std::vector<float> & verticesData, std::vector<UInt32> & indicesData);
		void Submit(std::vector<Vertex>& vertexData, std::vector<UInt32>& indexData);
		const Buffer & GetVertexBuffer() const { return *m_vertexBuffer; }
		const Buffer & GetIndexBuffer() const { return *m_indexBuffer; }
		UInt32 GetIndexCount() const { return m_indexCount; };
		VerticesInput & GetVerticesInput() { return m_verticesInput; }
	private:
		void InitBuffers();
	};
}