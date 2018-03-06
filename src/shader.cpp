#include "shader.h"
#include "vk_factory.hpp"
#include "utils.h"
#include "device.h"
#include <fstream>

namespace Concise
{
	Shader::Shader(ShaderType shaderType, std::string filename)
	{
		m_shaderType = shaderType;
		std::ifstream fin(filename, std::ios::binary | std::ios::in | std::ios::ate);
		
		if (fin.is_open())
		{
			Size shaderSize = fin.tellg();
			fin.seekg(0, std::ios::beg);
			char * shaderCode = new char[shaderSize];
			fin.read(shaderCode, shaderSize);
			fin.close();
			assert(shaderSize > 0);
			
			VkShaderModuleCreateInfo shaderModuleCreateInfo = 
				VkFactory::ShaderModuleCreateInfo(reinterpret_cast<UInt32*>(shaderCode), shaderSize);

			VK_CHECK_RESULT(vkCreateShaderModule(Device::Instance().GetLogicalDevice(), 
				&shaderModuleCreateInfo, NULL, &m_shaderModule));

			delete [] shaderCode;
		}
		else
		{
			assert(false);
			std::cerr << "Error: Could not open shader file \"" << filename << "\"" << std::endl;
		}
	}
	
	Shader::~Shader()
	{
		vkDestroyShaderModule(Device::Instance().GetLogicalDevice(), m_shaderModule, nullptr);
	}

	VkPipelineShaderStageCreateInfo Shader::PipelineShaderStageCreateInfo()
	{
		VkPipelineShaderStageCreateInfo pipelineShaderStageCreateInfo;
		switch (m_shaderType)
		{
		case VertexShader:
			pipelineShaderStageCreateInfo = VkFactory::PipelineShaderStageCreateInfo(VK_SHADER_STAGE_VERTEX_BIT, m_shaderModule);
		case FragmentShader:
			pipelineShaderStageCreateInfo = VkFactory::PipelineShaderStageCreateInfo(VK_SHADER_STAGE_FRAGMENT_BIT, m_shaderModule);
		default:
			assert(false);
		}

		return pipelineShaderStageCreateInfo;
	}
}