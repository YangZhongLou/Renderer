/*
* This code is licensed under the MIT license (MIT) (http://opensource.org/licenses/MIT)
*/

#include "shader.h"
#include "vk_factory.hpp"
#include "utils.h"
#include "device.h"
#include <fstream>

namespace Concise
{
	Shader::Shader(Device * device, ShaderType shaderType, std::string filename)
	{
		m_shaderType = shaderType;
		m_device = device;
		std::ifstream fin(filename, std::ios::binary | std::ios::in | std::ios::ate);
		
		if (fin.is_open())
		{
			Size shaderSize = fin.tellg();
			fin.seekg(0, std::ios::beg);
			char * shaderCode = new char[shaderSize];
			fin.read(shaderCode, shaderSize);
			fin.close();
			assert(shaderSize > 0);
			
			VkShaderModuleCreateInfo shaderModuleCreateInfo = VkFactory::ShaderModuleCreateInfo(reinterpret_cast<UInt32*>(shaderCode), shaderSize);

			VK_CHECK_RESULT(vkCreateShaderModule(device->GetLogicalDevice(), &shaderModuleCreateInfo, NULL, &m_shaderModule));

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
		vkDestroyShaderModule(m_device->GetLogicalDevice(), m_shaderModule, nullptr);
	}
}