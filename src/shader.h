/**
* Author ZhongLou Yang, 2018-2-5
*/
#pragma once

#include <string>
#include <vulkan/vulkan.h>

namespace Concise
{	
	class Device;
	
	enum ShaderType
	{
		VertexShader = VK_SHADER_STAGE_VERTEX_BIT,
		FragmentShader = VK_SHADER_STAGE_FRAGMENT_BIT,
	};
	
	class Shader
	{	
	private:
		VkShaderModule m_shaderModule;
		ShaderType m_shaderType;
	public:
		Shader(ShaderType shaderType, std::string filename);
		~Shader();
	public:
		VkShaderModule & GetModule() { return m_shaderModule; }
	};
}