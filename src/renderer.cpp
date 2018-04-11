
#include <thread>
#include "renderer.h"
#include "device.h"
#include "vk_factory.hpp"
#include "utils.h"
#include "uniforms.h"
#include "swapchain.h"
#include "vertices.h"
#include "debugger.h"
#include "shader.h"
#include "keycodes.hpp"
#include "vk_instance.h"
#include "defines.h"
#include "synchronizations.h"
#include "threadpool.h"

namespace Concise
{
	Renderer::Renderer()
	{
		m_swapchain = nullptr;
		
		m_uniforms = new Uniforms();
		m_threadPool = new ThreadPool(std::thread::hardware_concurrency());

		m_synchronizations = new Synchronizations();
	}
	
	Renderer::~Renderer()
	{
		SAFE_DELETE(m_uniforms);
		SAFE_DELETE(m_swapchain);
		SAFE_DELETE(m_threadPool);
	}

	void Renderer::Loop()
	{
		MSG msg;
		bool quitMessageReceived = false;
		while (!quitMessageReceived) {
			while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
				TranslateMessage(&msg);
				DispatchMessage(&msg);
				if (msg.message == WM_QUIT) {
					quitMessageReceived = true;
					break;
				}
			}

			RenderFrame();
		}

		vkDeviceWaitIdle(Device::Instance().GetLogicalDevice());
	}
	
	void Renderer::SubmitVerticesData(std::vector<Vertex> & verticesData, std::vector<UInt32> & indicesData)
	{
		Vertices::Instance().Submit(verticesData, indicesData);
	}
	
	void Renderer::RenderFrame()
	{
		
	}
}