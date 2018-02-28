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
	class Vertices;
	class Uniforms;
	class Swapchain;
	class Synchronizations;

	struct Vertex;
	
	class Renderer
	{
	private:				
		Vertices * m_vertices;
		Uniforms * m_uniforms;
		Swapchain * m_swapchain;
		Synchronizations * m_synchronizations;
				
		Camera m_camera;
	public:
		Renderer();
		~Renderer();
	public:
		void Loop();
		void RenderFrame();
		
		void SubmitVerticesData(std::vector<Vertex> & verticesData, std::vector<UInt32> & indicesData);
	};
}