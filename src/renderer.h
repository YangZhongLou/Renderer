
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
		std::vector<VkFramebuffer> m_frameBuffers;
		
		Device * m_device;
		
		UInt32 m_width;
		UInt32 m_Height;
		
		VkRenderPass m_renderPass;

	public:
		Renderer();
		~Renderer();
	public:
		void Init();
		void BuildCommandBuffers();
	private:
		void InitSync();
	}
}