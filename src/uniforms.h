/**
* Author ZhongLou Yang, 2018-2-5
*/
#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vulkan/vulkan.h>

namespace Concise
{
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

	public:
		Uniforms();
		~Uniforms();
		
	public:
		void UpdateVS(UInt32 width, UInt32 height);	
	public:
		const VkDescriptorBufferInfo & GetDescriptorBufferInfo() { return m_vsBuffer.descriptor; }
	};
}