#include "renderer.h"
#include "device.h"
#include "vkfactory.hpp"
#include "utils.h"
#include "uniforms.h"
#include "swapchain.h"

namespace Concise
{
	Renderer::Renderer()
	{
	}
	
	Renderer::~Renderer()
	{
		/** TODO, RAII */
		vkDestroySemaphore(m_device->GetLogicalDevice(), presentCompleteSemaphore, nullptr);
		vkDestroySemaphore(m_device->GetLogicalDevice(), m_renderSemaphore, nullptr);

		for (auto& fence : m_fences)
		{
			vkDestroyFence(m_device->GetLogicalDevice(), fence, nullptr);
		}
		
		SAFE_DELETE(m_vertice);
		SAFE_DELETE(m_device);
	}
	
	void Renderer::Init()
	{
		InitVulkan();
		InitVeritces();
		InitSync();
		InitVulkanInstance(true);
		InitPipelineCache();
		InitDepthStencil();
		InitFramebuffers();
		InitRenderPass();
		InitDescriptorPool();
		InitDescriptorSetLayout();
		InitDescriptorSet();
		InitPipelines();
	}
	
	void Renderer::InitVeritces()
	{
		m_vertices = new Vertices;
	}
	
	void Renderer::InitSync()
	{
		VkSemaphoreCreateInfo semaphoreCreateInfo = VkFactory::SemaphoreCreateInfo();
		Utils::VK_CHECK_RESULT(vkCreateSemaphore(m_device->GetLogicalDevice(), &semaphoreCreateInfo, nullptr, &m_presentSemaphore));
		Utils::VK_CHECK_RESULT(vkCreateSemaphore(m_device->GetLogicalDevice(), &semaphoreCreateInfo, nullptr, &m_renderSemaphore));
		
		VkFenceCreateInfo fenceCreateInfo = VkFactory::FenceCreateInfo(VK_FENCE_CREATE_SIGNALED_BIT);
		m_fences.resize(m_drawCmdBuffers.size());
		for (auto& fence : m_fences)
		{
			Utils::VK_CHECK_RESULT(vkCreateFence(m_device->GetLogicalDevice(), &fenceCreateInfo, nullptr, &fence));
		}
	}
	
	void Renderer::BuildCommandBuffers()
	{
		VkCommandBufferBeginInfo commandBufferBeginInfo = VkFactory::CommandBufferBeginInfo();
		
		VkClearValue clearValues[2];
		clearValues[0].color = { { 0.0f, 0.0f, 0.2f, 1.0f } };
		clearValues[1].m_depthStencil = { 1.0f, 0 };
		
		VkRenderPassBeginInfo renderPassBeginInfo = VkFactory::RenderPassBeginInfo();
		renderPassBeginInfo.renderPass = renderPass;
		renderPassBeginInfo.renderArea.offset.x = 0;
		renderPassBeginInfo.renderArea.offset.y = 0;
		renderPassBeginInfo.renderArea.extent.width = m_width;
		renderPassBeginInfo.renderArea.extent.height = m_height;
		/** TODO */
		renderPassBeginInfo.clearValueCount = 2;
		renderPassBeginInfo.pClearValues = clearValues;
		
		for (int32_t i = 0; i < m_drawCommandBuffers.size(); ++i)
		{
			renderPassBeginInfo.framebuffer = m_frameBuffers[i];

			Utils::VK_CHECK_RESULT(vkBeginCommandBuffer(m_drawCommandBuffers[i], &cmdBufInfo));

			vkCmdBeginRenderPass(m_drawCommandBuffers[i], &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

			VkViewport viewport = VkFactory::ViewPort(static_cast<float>m_width, static_cast<float>m_height, 0.0f, 1.0f);
			vkCmdSetViewport(m_drawCommandBuffers[i], 0, 1, &viewport);

			VkRect2D scissor = VkFactory::Scissor(m_width, m_height);
			vkCmdSetScissor(m_drawCommandBuffers[i], 0, 1, &scissor);

			vkCmdBindDescriptorSets(m_drawCommandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipelineLayout, 0, 1, &m_descriptorSet, 0, nullptr);

			vkCmdBindPipeline(m_drawCommandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipeline);

			VkDeviceSize offsets[1] = { 0 };
			vkCmdBindVertexBuffers(m_drawCommandBuffers[i], 0, 1, &vertices.GetVertexBufer().buffer, offsets);

			vkCmdBindIndexBuffer(m_drawCommandBuffers[i], vertices.GetIndexBuffer().buffer, 0, VK_INDEX_TYPE_UINT32);

			vkCmdDrawIndexed(m_drawCommandBuffers[i], vertices.GetIndexCount(), 1, 0, 0, 1);

			vkCmdEndRenderPass(m_drawCommandBuffers[i]);

			Utils::VK_CHECK_RESULT(vkEndCommandBuffer(m_drawCommandBuffers[i]));
		}
	}
	
	void Renderer::InitVulkan()
	{
		InitVulkanInstance(false);
	}
	
	void Renderer::InitVulkanInstance(bool enableValidation)
	{
		m_settings.validation = enableValidation;

		VkApplicationInfo appInfo = VkFactory::ApplicationInfo();
		std::vector<const char*> instanceExtensions = { VK_KHR_SURFACE_EXTENSION_NAME };

	#if defined(_WIN32)
		instanceExtensions.push_back(VK_KHR_WIN32_SURFACE_EXTENSION_NAME);
	#elif defined(VK_USE_PLATFORM_ANDROID_KHR)
		instanceExtensions.push_back(VK_KHR_ANDROID_SURFACE_EXTENSION_NAME);
	#elif defined(_DIRECT2DISPLAY)
		instanceExtensions.push_back(VK_KHR_DISPLAY_EXTENSION_NAME);
	#elif defined(VK_USE_PLATFORM_WAYLAND_KHR)
		instanceExtensions.push_back(VK_KHR_WAYLAND_SURFACE_EXTENSION_NAME);
	#elif defined(VK_USE_PLATFORM_XCB_KHR)
		instanceExtensions.push_back(VK_KHR_XCB_SURFACE_EXTENSION_NAME);
	#elif defined(VK_USE_PLATFORM_IOS_MVK)
		instanceExtensions.push_back(VK_MVK_IOS_SURFACE_EXTENSION_NAME);
	#elif defined(VK_USE_PLATFORM_MACOS_MVK)
		instanceExtensions.push_back(VK_MVK_MACOS_SURFACE_EXTENSION_NAME);
	#endif

		VkInstanceCreateInfo instanceCreateInfo = VkFactory::InstanceCreateInfo(appInfo);
		if (instanceExtensions.size() > 0)
		{
			if (m_settings.validation)
			{
				instanceExtensions.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
			}
			instanceCreateInfo.enabledExtensionCount = (uint32_t)instanceExtensions.size();
			instanceCreateInfo.ppEnabledExtensionNames = instanceExtensions.data();
		}
		if (m_settings.validation)
		{
			instanceCreateInfo.enabledLayerCount = vks::debug::validationLayerCount;
			instanceCreateInfo.ppEnabledLayerNames = vks::debug::validationLayerNames;
		}
		vkCreateInstance(&instanceCreateInfo, nullptr, &m_instance);
	}
	
	void Renderer::InitVulkanDevice()
	{
		m_device = new Device;
		m_device->Init();
	}
	
	void Renderer::InitDescriptorPool()
	{
		/** TODO, refactor this later */
		VkDescriptorPoolSize typeCounts[1];
		typeCounts[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		typeCounts[0].descriptorCount = 1;

		VkDescriptorPoolCreateInfo descriptorPoolInfo = VkFactory::DescriptorPoolCreateInfo();
		descriptorPoolInfo.poolSizeCount = 1;
		descriptorPoolInfo.pPoolSizes = typeCounts;
		descriptorPoolInfo.maxSets = 1;

		Utils::VK_CHECK_RESULT(vkCreateDescriptorPool(m_device->GetLogicalDevice(), &descriptorPoolInfo, nullptr, &m_descriptorPool));
	}
	
	void Renderer::InitDescriptorSetLayout()
	{
		VkDescriptorSetLayoutBinding layoutBinding = VkFactory::DescriptorSetLayoutBinding(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT);
		VkDescriptorSetLayoutCreateInfo descriptorLayoutCreateInfo = DescriptorSetLayoutCreateInfo(layoutBinding);

		Utils::VK_CHECK_RESULT(vkCreateDescriptorSetLayout(m_device->GetLogicalDevice(), &descriptorLayoutCreateInfo, nullptr, &m_descriptorSetLayout));
		VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo = VkFactory::PipelineLayoutCreateInfo(m_descriptorSetLayout);
		Utils::VK_CHECK_RESULT(vkCreatePipelineLayout(m_device->GetLogicalDevice(), &pipelineLayoutCreateInfo, nullptr, &m_pipelineLayout));
	}
	
	void Renderer::InitDescriptorSet()
	{
		VkDescriptorSetAllocateInfo descriptorSetAllocateInfo = DescriptorSetAllocateInfo(m_descriptorSetLayout);
		Utils::VK_CHECK_RESULT(vkAllocateDescriptorSets(m_device->GetLogicalDevice(), &descriptorSetAllocateInfo, &m_descriptorSet));

		VkWriteDescriptorSet writeDescriptorSet = VkFactory::WriteDescriptorSet(m_descriptorSet, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, &m_uniforms->GetDescriptorBufferInfo());
		vkUpdateDescriptorSets(m_device->GetLogicalDevice(), 1, &writeDescriptorSet, 0, nullptr);
	}
	
	void Renderer::InitDepthStencil()
	{
		VkImageCreateInfo image = VkFactory::ImageCreateInfo(m_depthFormat, {m_width, m_height, 1});
		Utils::VK_CHECK_RESULT(vkCreateImage(m_device->GetLogicalDevice(), &image, nullptr, &m_depthStencil.image));

		VkMemoryRequirements memReqs;
		vkGetImageMemoryRequirements(m_device->GetLogicalDevice(), m_depthStencil.image, &memReqs);
		VkMemoryAllocateInfo memAlloc = VkFactory::MemoryAllocateInfo(memReqs.size, m_device->GetMemoryTypeIndex(memReqs.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

		Utils::VK_CHECK_RESULT(vkAllocateMemory(m_device->GetLogicalDevice(), &memAlloc, nullptr, &m_depthStencil.mem));
		Utils::VK_CHECK_RESULT(vkBindImageMemory(m_device->GetLogicalDevice(), m_depthStencil.image, m_depthStencil.mem, 0));

		VkImageSubresourceRange imageSubresourceRange = VkFactory::ImageSubresourceRange(VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT);
		VkImageViewCreateInfo imageViewCreateInfo = VkFactory::ImageViewCreateInfo(m_depthFormat, m_depthStencil.image, imageSubresourceRange);
		Utils::VK_CHECK_RESULT(vkCreateImageView(m_device->GetLogicalDevice(), &imageViewCreateInfo, nullptr, &m_depthStencil.view));
	}
	
	void Renderer::InitFramebuffers()
	{
		m_frameBuffers.resize(m_swapchain->GetImageCount());
		for (size_t i = 0; i < m_frameBuffers.size(); i++)
		{
			std::vector<VkImageView> attachments;										
			attachments[0] = m_swapchain->GetBuffer(i).view;								
			attachments[1] = m_depthStencil.view;									

			VkFramebufferCreateInfo frameBufferCreateInfo = VkFactory::FramebufferCreateInfo(renderPass, attachments, m_width, m_height);
			frameBufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
			Utils::VK_CHECK_RESULT(vkCreateFramebuffer(m_device->GetLogicalDevice(), &frameBufferCreateInfo, nullptr, &m_frameBuffers[i]));
		}
	}
	
	void Renderer::InitRenderPass()
	{
		std::vector<VkAttachmentDescription> attachments = {};
		// Color attachment
		attachments[0] = VkFactory::AttachmentDescription(m_swapchain->GetColorFormat());
		attachments[0].storeOp = VK_ATTACHMENT_STORE_OP_STORE;							
		attachments[0].finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;					
																						
		// Depth attachment
		attachments[1] = VkFactory::AttachmentDescription(m_depthFormat);											
		attachments[1].finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;	

		// Setup attachment references
		VkAttachmentReference colorReference = VkFactory::AttachmentReference(0, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);			
		VkAttachmentReference depthReference = VkFactory::AttachmentReference(1, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);	

		VkSubpassDescription subpassDescription = VkFactory::SubpassDescription(VK_PIPELINE_BIND_POINT_GRAPHICS);
		subpassDescription.pColorAttachments = &colorReference;							
		subpassDescription.pDepthStencilAttachment = &depthReference;					

		std::vector<VkSubpassDependency> dependencies;
		VkFactory::SubpassDependencies(dependencies);

		VkRenderPassCreateInfo renderPassInfo = VkFactory::RenderPassCreateInfo(attachments, dependencies);
		renderPassInfo.subpassCount = 1;												
		renderPassInfo.pSubpasses = &subpassDescription;								

		Utils::VK_CHECK_RESULT(vkCreateRenderPass(m_device->GetLogicalDevice(), &renderPassInfo, nullptr, &m_renderPass));
	}
	
	void Renderer::InitPipelineCache()
	{
		VkPipelineCacheCreateInfo pipelineCacheCreateInfo = VkFactory::PipelineCacheCreateInfo();
		Utils::VK_CHECK_RESULT(vkCreatePipelineCache(m_device->GetLogicalDevice(), &pipelineCacheCreateInfo, nullptr, &m_pipelineCache))
	}
	
	void Renderer::InitPipelines()
	{
		VkPipelineInputAssemblyStateCreateInfo inputAssemblyState = VkFactory::PipelineInputAssemblyStateCreateInfo();
		VkPipelineRasterizationStateCreateInfo rasterizationState = VkFactory::PipelineRasterizationStateCreateInfo();
		
		VkPipelineRasterizationStateCreateInfo blendAttachmentState = VkFactory::VkPipelineColorBlendAttachmentState();
		std::vector<VkPipelineRasterizationStateCreateInfo> colorBlendStateAttachments;
		colorBlendStateAttachments.push_back(blendAttachmentState);
		VkPipelineColorBlendStateCreateInfo colorBlendState = VkFactory::PipelineColorBlendStateCreateInfo(colorBlendStateAttachments);

		VkPipelineViewportStateCreateInfo viewportState = VkFactory::PipelineViewportStateCreateInfo();

		std::vector<VkDynamicState> dynamicStateEnables;
		dynamicStateEnables.push_back(VK_DYNAMIC_STATE_VIEWPORT);
		dynamicStateEnables.push_back(VK_DYNAMIC_STATE_SCISSOR);
		VkPipelineDynamicStateCreateInfo dynamicState = VkFactory::PipelineDynamicStateCreateInfo(dynamicStateEnables);
		
		VkPipelineDepthStencilStateCreateInfo depthStencilState = VkFactory::PipelineDepthStencilStateCreateInfo();
		VkPipelineMultisampleStateCreateInfo multisampleState = VkFactory::PipelineMultisampleStateCreateInfo();
				
		std::vector<VkVertexInputBindingDescription> vertexInputBindingDecriptions;
		VkFactory::VertexInputBindingDescription(vertexInputBindingDecriptions);
		
		std::vector<VkVertexInputAttributeDescription> vertexInputAttributeDescriptions;
		VkFactory::VertexInputAttributeDescription(vertexInputAttributeDescriptions);

		VkPipelineVertexInputStateCreateInfo vertexInputState = VkFactory::PipelineVertexInputStateCreateInfo(vertexInputBindingDecriptions, vertexInputAttributeDescriptions);

		// Shaders
		std::array<VkPipelineShaderStageCreateInfo, 2> shaderStages{};
		Shader vertexShader(m_device, "shaders/triangle/triangle.vert.spv"")
		Shader fragmentShader(m_device, "shaders/triangle/triangle.frag.spv");
		shaderStages[0] = VkFactory::PipelineShaderStageCreateInfo(VK_SHADER_STAGE_VERTEX_BIT, vertexShader);
		shaderStages[1] = VkFactory::PipelineShaderStageCreateInfo(VK_SHADER_STAGE_FRAGMENT_BIT, fragmentShader);

		VkGraphicsPipelineCreateInfo graphicsPipelineCreateInfo = VkFactory::GraphicsPipelineCreateInfo(shaderStages,
			&vertexInputState,
			&inputAssemblyState,
			&viewportState,
			&rasterizationState,
			&multisampleState,
			&depthStencilState,
			&colorBlendState,
			&dynamicState,
			m_pipelineLayout,
			m_renderPass
			);
		
		Utils::VK_CHECK_RESULT(vkCreateGraphicsPipelines(device, m_pipelineCache, 1, &graphicsPipelineCreateInfo, nullptr, &m_pipeline));
	}
	
	void Renderer::InitWindow(HINSTANCE hinstance, WNDPROC wndproc)
	{
		m_windowInstance = hinstance;

		WNDCLASSEX wndClass;

		wndClass.cbSize = sizeof(WNDCLASSEX);
		wndClass.style = CS_HREDRAW | CS_VREDRAW;
		wndClass.lpfnWndProc = wndproc;
		wndClass.cbClsExtra = 0;
		wndClass.cbWndExtra = 0;
		wndClass.hInstance = hinstance;
		wndClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
		wndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
		wndClass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
		wndClass.lpszMenuName = NULL;
		wndClass.lpszClassName = name.c_str();
		wndClass.hIconSm = LoadIcon(NULL, IDI_WINLOGO);

		if (!RegisterClassEx(&wndClass))
		{
			std::cout << "Could not register window class!\n";
			fflush(stdout);
			exit(1);
		}

		int screenWidth = GetSystemMetrics(SM_CXSCREEN);
		int screenHeight = GetSystemMetrics(SM_CYSCREEN);

		if (m_settings.fullscreen)
		{
			DEVMODE dmScreenSettings;
			memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));
			dmScreenSettings.dmSize = sizeof(dmScreenSettings);
			dmScreenSettings.dmPelsWidth = screenWidth;
			dmScreenSettings.dmPelsHeight = screenHeight;
			dmScreenSettings.dmBitsPerPel = 32;
			dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

			if ((m_width != (uint32_t)screenWidth) && (m_height != (uint32_t)screenHeight))
			{
				if (ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN) != DISP_CHANGE_SUCCESSFUL)
				{
					if (MessageBox(NULL, "Fullscreen Mode not supported!\n Switch to window mode?", "Error", MB_YESNO | MB_ICONEXCLAMATION) == IDYES)
					{
						m_settings.fullscreen = false;
					}
					else
					{
						return nullptr;
					}
				}
			}

		}

		DWORD dwExStyle;
		DWORD dwStyle;

		if (m_settings.fullscreen)
		{
			dwExStyle = WS_EX_APPWINDOW;
			dwStyle = WS_POPUP | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;
		}
		else
		{
			dwExStyle = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;
			dwStyle = WS_OVERLAPPEDWINDOW | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;
		}

		RECT windowRect;
		windowRect.left = 0L;
		windowRect.top = 0L;
		windowRect.right = m_settings.fullscreen ? (long)screenWidth : (long)width;
		windowRect.bottom = m_settings.fullscreen ? (long)screenHeight : (long)height;

		AdjustWindowRectEx(&windowRect, dwStyle, FALSE, dwExStyle);

		std::string windowTitle = getWindowTitle();
		m_window = CreateWindowEx(0,
			name.c_str(),
			windowTitle.c_str(),
			dwStyle | WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
			0,
			0,
			windowRect.right - windowRect.left,
			windowRect.bottom - windowRect.top,
			NULL,
			NULL,
			hinstance,
			NULL);

		if (!m_settings.fullscreen)
		{
			// Center on screen
			uint32_t x = (GetSystemMetrics(SM_CXSCREEN) - windowRect.right) / 2;
			uint32_t y = (GetSystemMetrics(SM_CYSCREEN) - windowRect.bottom) / 2;
			SetWindowPos(window, 0, x, y, 0, 0, SWP_NOZORDER | SWP_NOSIZE);
		}

		if (!window)
		{
			printf("Could not create window!\n");
			fflush(stdout);
			return nullptr;
			exit(1);
		}

		ShowWindow(window, SW_SHOW);
		SetForegroundWindow(window);
		SetFocus(window);

		return window;
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
	}
	
	void Renderer::SubmitVerticesData(std::vector<Vertex> & verticesData, std::vector<UInt32> & indicesData)
	{
		m_vertices->Submit(verticesData, indicesData);
	}
	
	void Renderer::RenderFrame()
	{
		Utils::VK_CHECK_RESULT(swapChain.acquireNextImage(m_presentSemaphore, &m_currentBuffer));

		Utils::VK_CHECK_RESULT(vkWaitForFences(device, 1, &waitFences[m_currentBuffer], VK_TRUE, UINT64_MAX));
		Utils::VK_CHECK_RESULT(vkResetFences(device, 1, &waitFences[m_currentBuffer]));

		VkPipelineStageFlags waitStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		
		VkSubmitInfo submitInfo = VkFactory::SubmitInfo();
		submitInfo.pWaitDstStageMask = &waitStageMask;									
		submitInfo.pWaitSemaphores = &m_presentSemaphore;							
		submitInfo.waitSemaphoreCount = 1;																														
		submitInfo.pSignalSemaphores = &m_renderSemaphore;						
		submitInfo.signalSemaphoreCount = 1;											
		submitInfo.pCommandBuffers = &m_drawCommandBuffers[m_currentBuffer];					
		submitInfo.commandBufferCount = 1;												

		// Submit to the graphics queue passing a wait fence
		Utils::VK_CHECK_RESULT(vkQueueSubmit(queue, 1, &submitInfo, waitFences[m_currentBuffer]));
		
		// Present the current buffer to the swap chain
		// Pass the semaphore signaled by the command buffer submission from the submit info as the wait semaphore for swap chain presentation
		// This ensures that the image is not presented to the windowing system until all commands have been submitted
		VK_CHECK_RESULT(swapChain.queuePresent(queue, m_currentBuffer, m_renderSemaphore));
	}
}