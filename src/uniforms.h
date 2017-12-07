
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vulkan/vulkan.h>

namespace Concise
{
	class Device;
	
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
	public:
		Uniforms(Device * device);
		~Uniforms();
		
	public:
		void Init();
		void UpdateVS();		
	}
}