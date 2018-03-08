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
	class Descriptors;

	struct PushConstants
	{
		float constants[16];
	};

	class Shaders
	{
	private:
		std::vector<Shader*> m_shaders;
		std::vector<VkPipelineShaderStageCreateInfo> m_shaderStages;
		Descriptors * m_descriptors;
	public:
		static Shaders & Instance() 
		{
			static Shaders instance;
			return instance;
		}
	private:
		Shaders();
		~Shaders();
	public:
		std::vector<VkPipelineShaderStageCreateInfo> & GetShaderStages() 
		{
			return m_shaderStages;
		}

		inline VkDescriptorSetLayout & GetDescriptorSetLayout();
	};
}