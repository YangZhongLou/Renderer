/**
* Author ZhongLou Yang, 2018-2-5
*/
#pragma once

#include <string>
#include <vulkan/vulkan.h>
#include <vector>

namespace Concise
{
	class Shader;
	class Device;
	enum ShaderType;
	
	struct ShaderResource
	{
		ShaderType shaderType;
		std::string filename;
	}
	
	struct PipelineShaders
	{
		std::vector<Shader> m_shaders;
		
		PipelineShaders(Device * device, std::vector<ShaderResource> & shaderResources)
		{
			for (auto shaderResource : shaderResources)
			{
				m_shaders.push_back(Shader(device, shaderResource.shaderType, shaderResource.filename);
			}
		}
		
		~PipelineShaders()
		{
			m_shaders.clear();
		}
	};

	class Shaders
	{
	private:
		std::vector<PipelineShaders> m_pipelinesShaders;
	public:
		Shaders(PipelineShaders & pipelineShaders);
		~Shaders();
	};
}