/*
* This code is licensed under the MIT license (MIT) (http://opensource.org/licenses/MIT)
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
		Device * m_device;
		ShaderType m_shaderType;
	public:
		Shader(Device * device, ShaderType shaderType, std::string filename);
		~Shader();
	public:
		VkShaderModule & GetModule() { return m_shaderModule; }
	};
}