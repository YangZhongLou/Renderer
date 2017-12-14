
/*
* This code is licensed under the MIT license (MIT) (http://opensource.org/licenses/MIT)
*/
#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vulkan/vulkan.h>

namespace Concise
{
	class Device;
	class Renderer;
	
	class Uniforms
	{
	private:
		struct 
		{
			glm::mat4 projectionMatrix;
			glm::mat4 modelMatrix;
			glm::mat4 viewMatrix;
		} m_vsData;
		
		struct 
		{
			VkDeviceMemory memory;		
			VkBuffer buffer;			
			VkDescriptorBufferInfo descriptor;
		} m_vsBuffer;
		
		Device * m_device;
		Renderer * m_renderer;
	public:
		Uniforms(Device * device, Renderer * renderer);
		~Uniforms();
		
	public:
		void Init();
		void UpdateVS();	
	public:
		const VkDescriptorBufferInfo & GetDescriptorBufferInfo() { return m_vsBuffer.descriptor; }
	};
}