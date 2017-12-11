/*
* This code is licensed under the MIT license (MIT) (http://opensource.org/licenses/MIT)
*/

#include "shader.h"
#include "vkfactory.hpp"
#include "utils.h"

namespace Concise
{
	Shader::Shader(Device * device, std::string& filename)
	{
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
			
			VkShaderModuleCreateInfo shaderModuleCreateInfo = VkFactory::ShaderModuleCreateInfo(static_cast(UInt32)(shaderCode), shaderSize);

			Utils::VK_CHECK_RESULT(vkCreateShaderModule(device, &moduleCreateInfo, NULL, &m_shaderModule));

			delete [] shaderCode;
		}
		else
		{
			std::cerr << "Error: Could not open shader file \"" << filename << "\"" << std::endl;
		}
	}
	
	Shader::~Shader()
	{
		vkDestroyShaderModule(m_device->GetLoicalDevice(), m_shaderModule, nullptr);
	}
}