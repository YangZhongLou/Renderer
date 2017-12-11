/*
* This code is licensed under the MIT license (MIT) (http://opensource.org/licenses/MIT)
*/

#include <string>
#include <vulkan/vulkan.h>

namespace Concise
{	
	class Device;
	
	class Shader
	{	
	private:
		VkShaderModule m_shaderModule;
		Device * m_device;
	public:
		Shader(Device * device, std::string& filename);
		~Shader();
	public:
		VkShaderModule GetModule() { return m_shaderModule; }
	}
}