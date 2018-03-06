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

	class Shaders
	{
	private:
		std::vector<Shader*> m_shaders;
		std::vector<VkPipelineShaderStageCreateInfo> m_shaderStages;
	public:
		Shaders();
		~Shaders();
	public:
		std::vector<VkPipelineShaderStageCreateInfo> & GetShaderStages();
	};
}