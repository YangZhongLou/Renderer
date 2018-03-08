/**
* Author ZhongLou Yang, 2018-2-5
*/
#pragma once

#include <vulkan/vulkan.h>
#include <vector>
#include "types.h"
#include "camera.hpp"

namespace Concise
{
	class Uniforms;
	class Swapchain;
	class Synchronizations;
	class ThreadPool;
	class Pipelines;
	class Renderpass;

	struct Vertex;
	
	class Renderer
	{
	private:				
		Uniforms * m_uniforms;
		Swapchain * m_swapchain;
		Synchronizations * m_synchronizations;
		ThreadPool * m_threadPool;
		Renderpass * m_renderpass;
		Pipelines * m_pipelines;

		struct ThreadData 
		{
			VkCommandPool commandPool;
			std::vector<VkCommandBuffer> commandBuffer;
			std::vector<PushConstants> pushConstBlock;
		};
		std::vector<ThreadData> m_threadData;
				
		Camera m_camera;
	public:
		static Renderer & Instance()
		{
			static Renderer instance;
			return instance;
		}
	private:
		Renderer();
		~Renderer();
	public:
		Renderpass * GetRenderpass() const{ return m_renderpass; }
	public:
		void Loop();
		void RenderFrame();
		void SubmitVerticesData(std::vector<Vertex> & verticesData, std::vector<UInt32> & indicesData);
	};
}