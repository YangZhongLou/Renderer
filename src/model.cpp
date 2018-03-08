
#include <assimp/Importer.hpp> 
#include <assimp/scene.h>     
#include <assimp/postprocess.h>
#include <assimp/cimport.h>
#include "model.h"
#include "vertices.h"

namespace Concise
{
	Model::Model(const std::string & filename)
	{
		m_filename = std::move(filename);		
	}
	
	Model::~Model()
	{
	}
	
	void Model::LoadData(VertexLayout & layout, ModelCreateInfo * modelCreateInfo, int flags)
	{
		Assimp::Importer importer;
		const aiScene * scene = importer.ReadFile(m_filename.c_str(), flags);
		
		if (scene)
		{
			m_parts.clear();
			m_parts.resize(scene->mNumMeshes);

			glm::vec3 scale(1.0f);
			glm::vec2 uvscale(1.0f);
			glm::vec3 center(0.0f);

			if (modelCreateInfo)
			{
				scale = modelCreateInfo->scale;
				uvscale = modelCreateInfo->uvscale;
				center = modelCreateInfo->center;
			}

			m_vertexCount = 0;
			m_indexCount = 0;

			for (UInt32 i = 0; i < scene->mNumMeshes; i++)
			{
				const aiMesh * mesh = scene->mMeshes[i];

				m_parts[i] = {};
				m_parts[i].vertexBase = m_vertexCount;
				m_parts[i].indexBase = m_indexCount;

				m_vertexCount += scene->mMeshes[i]->mNumVertices;

				aiColor3D pColor(0.f, 0.f, 0.f);
				scene->mMaterials[mesh->mMaterialIndex]->Get(AI_MATKEY_COLOR_DIFFUSE, pColor);

				const aiVector3D zero3D(0.0f, 0.0f, 0.0f);

				for (UInt32 j = 0; j < mesh->mNumVertices; j++)
				{
					const aiVector3D * pos = &(mesh->mVertices[j]);
					const aiVector3D * normal = &(mesh->mNormals[j]);
					const aiVector3D * texCoord = (mesh->HasTextureCoords(0)) ? &(mesh->mTextureCoords[0][j]) : &zero3D;
					const aiVector3D * tangent = (mesh->HasTangentsAndBitangents()) ? &(mesh->mTangents[j]) : &zero3D;
					const aiVector3D * biTangent = (mesh->HasTangentsAndBitangents()) ? &(mesh->mBitangents[j]) : &zero3D;

					for (auto& component : layout.components)
					{
						switch (component) {
						case VERTEX_COMPONENT_POSITION:
							m_vertexData.push_back(pos->x * scale.x + center.x);
							m_vertexData.push_back(-pos->y * scale.y + center.y);
							m_vertexData.push_back(pos->z * scale.z + center.z);
							break;
						case VERTEX_COMPONENT_NORMAL:
							m_vertexData.push_back(normal->x);
							m_vertexData.push_back(-normal->y);
							m_vertexData.push_back(normal->z);
							break;
						case VERTEX_COMPONENT_UV:
							m_vertexData.push_back(texCoord->x * uvscale.s);
							m_vertexData.push_back(texCoord->y * uvscale.t);
							break;
						case VERTEX_COMPONENT_COLOR:
							m_vertexData.push_back(pColor.r);
							m_vertexData.push_back(pColor.g);
							m_vertexData.push_back(pColor.b);
							break;
						case VERTEX_COMPONENT_TANGENT:
							m_vertexData.push_back(tangent->x);
							m_vertexData.push_back(tangent->y);
							m_vertexData.push_back(tangent->z);
							break;
						case VERTEX_COMPONENT_BITANGENT:
							m_vertexData.push_back(biTangent->x);
							m_vertexData.push_back(biTangent->y);
							m_vertexData.push_back(biTangent->z);
							break;
						case VERTEX_COMPONENT_DUMMY_FLOAT:
							m_vertexData.push_back(0.0f);
							break;
						case VERTEX_COMPONENT_DUMMY_VEC4:
							m_vertexData.push_back(0.0f);
							m_vertexData.push_back(0.0f);
							m_vertexData.push_back(0.0f);
							m_vertexData.push_back(0.0f);
							break;
						};
					}

					m_dimension.max.x = fmax(pos->x, m_dimension.max.x);
					m_dimension.max.y = fmax(pos->y, m_dimension.max.y);
					m_dimension.max.z = fmax(pos->z, m_dimension.max.z);

					m_dimension.min.x = fmin(pos->x, m_dimension.min.x);
					m_dimension.min.y = fmin(pos->y, m_dimension.min.y);
					m_dimension.min.z = fmin(pos->z, m_dimension.min.z);
				}

				m_dimension.size = m_dimension.max - m_dimension.min;

				m_parts[i].vertexCount = mesh->mNumVertices;

				UInt32 indexBase = static_cast<UInt32>(m_indexData.size());
				for (UInt32 j = 0; j < mesh->mNumFaces; j++)
				{
					const aiFace & face = mesh->mFaces[j];
					if (face.mNumIndices != 3)
						continue;
					
					m_indexData.push_back(indexBase + face.mIndices[0]);
					m_indexData.push_back(indexBase + face.mIndices[1]);
					m_indexData.push_back(indexBase + face.mIndices[2]);
					m_parts[i].indexCount += 3;
					m_indexCount += 3;
				}
			}
		}
		else
		{
			assert(false);
		}
	}
}

