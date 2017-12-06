
#include <vulkan/vulkan.h>
#include <vector>
#include "types.h"

namespace Concise
{
	class Device;
	
	class Renderer
	{
	private:
		VkSemaphoreCreateInfo m_presentSemaphore;
		VkSemaphoreCreateInfo m_renderSemaphore;
		std::vector<VkFence> m_fences;
		
		std::vector<VkCommandBuffer> m_drawCommandBuffers;
		
		Device * m_device;
		
		UInt32 m_width;
		UInt32 m_Height;
	public:
		Renderer();
		~Renderer();
	public:
		void Init();
	private:
		void InitSync();
	}
}