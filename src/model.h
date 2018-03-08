/**
* Author ZhongLou Yang, 2018-2-5
*/

#include <vector>
#include <string>
#include <glm/glm.hpp>
#include "types.h"

namespace Concise
{
	class Device;
	struct VertexLayout;
	
	struct ModelCreateInfo 
	{
		glm::vec3 center;
		glm::vec3 scale;
		glm::vec2 uvscale;

		ModelCreateInfo() {};

		ModelCreateInfo(glm::vec3 scale, glm::vec2 uvscale, glm::vec3 center)
		{
			this->center = center;
			this->scale = scale;
			this->uvscale = uvscale;
		}

		ModelCreateInfo(float scale, float uvscale, float center)
		{
			this->center = glm::vec3(center);
			this->scale = glm::vec3(scale);
			this->uvscale = glm::vec2(uvscale);
		}
	};
	
	class Model
	{
	private:
		std::string m_filename;
		std::vector<float> m_vertexData;
		std::vector<UInt32> m_indexData;
		UInt32 m_vertexCount = 0;
		UInt32 m_indexCount = 0;
		
		struct ModelPart 
		{
			UInt32 vertexBase;
			UInt32 vertexCount;
			UInt32 indexBase;
			UInt32 indexCount;
		};
		
		struct Dimension
		{
			glm::vec3 min = glm::vec3(FLT_MAX);
			glm::vec3 max = glm::vec3(-FLT_MAX);
			glm::vec3 size;
		} m_dimension;
		
		std::vector<ModelPart> m_parts;
		
		static const int defaultFlags = aiProcess_FlipWindingOrder | aiProcess_Triangulate | aiProcess_PreTransformVertices | aiProcess_CalcTangentSpace | aiProcess_GenSmoothNormals;
	public:
		Model(const std::string & filename);
		~Model();
	public:
		void LoadData(VertexLayout & layout, ModelCreateInfo & modelCreateInfo, int flags = defaultFlags);
	};
	
}