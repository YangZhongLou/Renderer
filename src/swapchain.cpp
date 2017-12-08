#include "swapchain.h"

namespace Concise
{
	Swapchain::Swapchain(VkInstance instance, Device * device) : m_instance(instance), m_device(device)
	{
	}
	
	Swapchain::~Swapchain()
	{
	}
	
	void Swapchain::Init()
	{
		InitDllFunction();
		InitSurface();
	}
	
	void Swapchain::InitDllFunction()
	{
		
	}
	
	void Swapchain::InitSurface()
	{
	}
	

}