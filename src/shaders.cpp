/*
* This code is licensed under the MIT license (MIT) (http://opensource.org/licenses/MIT)
*/

#include "shaders.h"
#include "vk_factory.hpp"
#include "utils.h"
#include "device.h"

namespace Concise
{
	Shaders(PipelineShaders & pipelineShaders)
	{
		m_pipelinesShaders.push_back(pipelineShaders);
	}
	
	~Shader()
	{
		m_pipelinesShaders.clear();
	}
}