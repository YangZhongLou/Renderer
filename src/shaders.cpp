#include "shaders.h"
#include "vk_factory.hpp"
#include "utils.h"
#include "device.h"
#include "shader.h"
#include "descriptors.h"

namespace Concise
{
	Shaders::Shaders()
	{
		m_shaders.push_back(new Shader(VertexShader, "shaders/multithreading/phong.vert.spv"));
		m_shaders.push_back(new Shader(FragmentShader, "shaders/multithreading/phong.frag.spv"));

		m_shaderStages.resize(m_shaders.size());
		for (auto i = 0; i < m_shaders.size(); ++i)
		{
			m_shaderStages[i] = m_shaders[i]->PipelineShaderStageCreateInfo();
		}

		m_descriptors = new Descriptors;
	}
	
	Shaders::~Shaders()
	{
		for (auto shader : m_shaders)
		{
			SAFE_DELETE(shader);
		}

		SAFE_DELETE(m_descriptors);
	}

	inline VkDescriptorSetLayout & Shaders::GetDescriptorSetLayout()
	{
		return m_descriptors->GetDescriptorSetLayout();
	}
}