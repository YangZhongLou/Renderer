#include "renderer.h"
#include "device.h"
#include "vkfactory.hpp"
#include "utils.h"
#include "uniforms.h"
#include "swapchain.h"
#include "vertices.h"
#include "debugger.h"
#include "shader.h"
#include "keycodes.hpp"

namespace Concise
{
	Renderer::Renderer()
	{
		m_swapchain = nullptr;
	}
	
	Renderer::~Renderer()
	{
		DestroyCommandBuffers();

		if (m_descriptorPool != VK_NULL_HANDLE)
		{
			vkDestroyDescriptorPool(m_device->GetLogicalDevice(), m_descriptorPool, nullptr);
		}

		vkDestroyRenderPass(m_device->GetLogicalDevice(), m_renderPass, nullptr);

		for (UInt32 i = 0; i < m_framebuffers.size(); i++)
		{
			vkDestroyFramebuffer(m_device->GetLogicalDevice(), m_framebuffers[i], nullptr);
		}

		/** TODO, RAII */
		vkDestroySemaphore(m_device->GetLogicalDevice(), m_presentCompleteSemaphore, nullptr);
		vkDestroySemaphore(m_device->GetLogicalDevice(), m_renderCompleteSemaphore, nullptr);

		for (auto& fence : m_fences)
		{
			vkDestroyFence(m_device->GetLogicalDevice(), fence, nullptr);
		}
		
		vkDestroyPipeline(m_device->GetLogicalDevice(), m_pipeline, nullptr);

		vkDestroyPipelineLayout(m_device->GetLogicalDevice(), m_pipelineLayout, nullptr);
		vkDestroyDescriptorSetLayout(m_device->GetLogicalDevice(), m_descriptorSetLayout, nullptr);
		vkDestroyPipelineCache(m_device->GetLogicalDevice(), m_pipelineCache, nullptr);

		vkDestroyImageView(m_device->GetLogicalDevice(), m_depthStencil.view, nullptr);
		vkDestroyImage(m_device->GetLogicalDevice(), m_depthStencil.image, nullptr);
		vkFreeMemory(m_device->GetLogicalDevice(), m_depthStencil.mem, nullptr);

		SAFE_DELETE(m_uniforms);
		SAFE_DELETE(m_vertices);
		SAFE_DELETE(m_swapchain);
		SAFE_DELETE(m_debugger);
		SAFE_DELETE(m_device);

		vkDestroyInstance(m_vkInstance, nullptr);
	}
	
	void Renderer::Init()
	{
		m_prepared = false;
		InitConsole("Concise");
		InitVulkan();
		InitUniforms();
		InitVeritces();
		InitDepthStencil();
		InitRenderPass();
		InitDescriptorSetLayout();
		InitDescriptorPool();
		InitDescriptorSet();
		InitPipelineCache();
		InitFramebuffers();
		InitPipelines();
		m_prepared = true;
	}
	
	void Renderer::InitVeritces()
	{
		m_vertices = new Vertices(m_device);
	}
	
	void Renderer::DestroyCommandBuffers()
	{
		vkFreeCommandBuffers(m_device->GetLogicalDevice(), m_device->GetCommandPool(), static_cast<UInt32>(m_drawCmdBuffers.size()), m_drawCmdBuffers.data());
	}

	void Renderer::CreateCommandBuffers()
	{
		m_drawCmdBuffers.resize(m_swapchain->GetImageCount());

		VkCommandBufferAllocateInfo cmdBufAllocateInfo = VkFactory::CommandBufferAllocateInfo(m_device->GetCommandPool(), VK_COMMAND_BUFFER_LEVEL_PRIMARY, static_cast<UInt32>(m_drawCmdBuffers.size()));

		VK_CHECK_RESULT(vkAllocateCommandBuffers(m_device->GetLogicalDevice(), &cmdBufAllocateInfo, m_drawCmdBuffers.data()));
	}

	void Renderer::BuildCommandBuffers()
	{
		VkCommandBufferBeginInfo commandBufferBeginInfo = VkFactory::CommandBufferBeginInfo();
		
		VkClearValue clearValues[2];
		clearValues[0].color = { { 0.0f, 0.0f, 0.2f, 1.0f } };
		clearValues[1].depthStencil = { 1.0f, 0 };
		
		VkRenderPassBeginInfo renderPassBeginInfo = VkFactory::RenderPassBeginInfo();
		renderPassBeginInfo.renderPass = m_renderPass;
		renderPassBeginInfo.renderArea.offset.x = 0;
		renderPassBeginInfo.renderArea.offset.y = 0;
		renderPassBeginInfo.renderArea.extent.width = m_width;
		renderPassBeginInfo.renderArea.extent.height = m_height;
		/** TODO */
		renderPassBeginInfo.clearValueCount = 2;
		renderPassBeginInfo.pClearValues = clearValues;
		
		for (Int32 i = 0; i < m_drawCmdBuffers.size(); ++i)
		{
			renderPassBeginInfo.framebuffer = m_framebuffers[i];

			VK_CHECK_RESULT(vkBeginCommandBuffer(m_drawCmdBuffers[i], &commandBufferBeginInfo));

			vkCmdBeginRenderPass(m_drawCmdBuffers[i], &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

			VkDeviceSize offsets[1] = { 0 };
			vkCmdBindVertexBuffers(m_drawCmdBuffers[i], 0, 1, &m_vertices->GetVertexBuffer().buffer, offsets);
			vkCmdBindIndexBuffer(m_drawCmdBuffers[i], m_vertices->GetIndexBuffer().buffer, 0, VK_INDEX_TYPE_UINT32);
			
			VkRect2D scissor = VkFactory::Scissor(m_width, m_height);
			vkCmdSetScissor(m_drawCmdBuffers[i], 0, 1, &scissor);

			vkCmdBindDescriptorSets(m_drawCmdBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipelineLayout, 0, 1, &m_descriptorSet, 0, nullptr);
			
			for (Int32 j = 0; j < m_viewports.size(); ++j)
			{
				vkCmdSetViewport(m_drawCmdBuffers[i], 0, 1, &m_viewports[i]);

				vkCmdBindPipeline(m_drawCmdBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipelines[i]);

				vkCmdDrawIndexed(m_drawCmdBuffers[i], m_vertices->GetIndexCount(), 1, 0, 0, 1);
			}
			
			vkCmdEndRenderPass(m_drawCmdBuffers[i]);

			VK_CHECK_RESULT(vkEndCommandBuffer(m_drawCmdBuffers[i]));
		}
	}
	
	void Renderer::InitVulkan()
	{
		InitVulkanInstance(true);
		InitVulkanDebugger();
		InitVulkanDevice();
		InitSwapchain();
		InitCommandBuffers();
		InitVulkanSync();
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
			instanceCreateInfo.enabledExtensionCount = static_cast<UInt32>(instanceExtensions.size());
			instanceCreateInfo.ppEnabledExtensionNames = instanceExtensions.data();
		}
		if (m_settings.validation)
		{
			instanceCreateInfo.enabledLayerCount = Debugger::validationLayerCount;
			instanceCreateInfo.ppEnabledLayerNames = Debugger::validationLayerNames;
		}
		VK_CHECK_RESULT(vkCreateInstance(&instanceCreateInfo, nullptr, &m_vkInstance));
	}
	
	void Renderer::InitVulkanDebugger()
	{
		if (!m_settings.validation)
		{
			return;
		}

		VkDebugReportFlagsEXT debugReportFlags = VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT;
		m_debugger = new Debugger(m_vkInstance, debugReportFlags);
		m_debugger->Init();
	}

	void Renderer::InitVulkanDevice()
	{
		m_device = new Device(m_vkInstance);
		m_device->Init();
	}
	
	void Renderer::InitSwapchain()
	{
		m_swapchain = new Swapchain(m_vkInstance, m_device, this);
		m_swapchain->Init();
		m_swapchain->CreateSwapchain(&m_width, &m_height, m_settings.vsync);
	}

	void Renderer::InitCommandBuffers()
	{
		CreateCommandBuffers();
	}

	void Renderer::InitVulkanSync()
	{
		VkSemaphoreCreateInfo semaphoreCreateInfo = VkFactory::SemaphoreCreateInfo();
		VK_CHECK_RESULT(vkCreateSemaphore(m_device->GetLogicalDevice(), &semaphoreCreateInfo, nullptr, &m_presentCompleteSemaphore));
		VK_CHECK_RESULT(vkCreateSemaphore(m_device->GetLogicalDevice(), &semaphoreCreateInfo, nullptr, &m_renderCompleteSemaphore));

		VkFenceCreateInfo fenceCreateInfo = VkFactory::FenceCreateInfo(VK_FENCE_CREATE_SIGNALED_BIT);
		m_fences.resize(m_drawCmdBuffers.size());
		for (auto& fence : m_fences)
		{
			VK_CHECK_RESULT(vkCreateFence(m_device->GetLogicalDevice(), &fenceCreateInfo, nullptr, &fence));
		}
	}

	void Renderer::InitUniforms()
	{
		m_uniforms = new Uniforms(m_device, this);
		m_uniforms->Init();
		m_uniforms->UpdateVS();
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

		VK_CHECK_RESULT(vkCreateDescriptorPool(m_device->GetLogicalDevice(), &descriptorPoolInfo, nullptr, &m_descriptorPool));
	}
	
	void Renderer::InitDescriptorSetLayout()
	{
		VkDescriptorSetLayoutBinding layoutBinding = VkFactory::DescriptorSetLayoutBinding(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT);
		VkDescriptorSetLayoutCreateInfo descriptorLayoutCreateInfo = VkFactory::DescriptorSetLayoutCreateInfo(layoutBinding);

		VK_CHECK_RESULT(vkCreateDescriptorSetLayout(m_device->GetLogicalDevice(), &descriptorLayoutCreateInfo, nullptr, &m_descriptorSetLayout));
		VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo = VkFactory::PipelineLayoutCreateInfo(m_descriptorSetLayout);
		VK_CHECK_RESULT(vkCreatePipelineLayout(m_device->GetLogicalDevice(), &pipelineLayoutCreateInfo, nullptr, &m_pipelineLayout));
	}
	
	void Renderer::InitDescriptorSet()
	{
		VkDescriptorSetAllocateInfo descriptorSetAllocateInfo = VkFactory::DescriptorSetAllocateInfo(m_descriptorSetLayout, m_descriptorPool);
		VK_CHECK_RESULT(vkAllocateDescriptorSets(m_device->GetLogicalDevice(), &descriptorSetAllocateInfo, &m_descriptorSet));

		VkWriteDescriptorSet writeDescriptorSet = VkFactory::WriteDescriptorSet(m_descriptorSet, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, m_uniforms->GetDescriptorBufferInfo());
		vkUpdateDescriptorSets(m_device->GetLogicalDevice(), 1, &writeDescriptorSet, 0, nullptr);
	}
	
	void Renderer::InitDepthStencil()
	{
		VkImageCreateInfo image = VkFactory::ImageCreateInfo(m_device->GetSupportedDepthFormat(), {m_width, m_height, 1});
		VK_CHECK_RESULT(vkCreateImage(m_device->GetLogicalDevice(), &image, nullptr, &m_depthStencil.image));

		VkMemoryRequirements memReqs;
		vkGetImageMemoryRequirements(m_device->GetLogicalDevice(), m_depthStencil.image, &memReqs);
		VkMemoryAllocateInfo memAlloc = VkFactory::MemoryAllocateInfo(memReqs.size, m_device->GetMemoryTypeIndex(memReqs.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT));

		VK_CHECK_RESULT(vkAllocateMemory(m_device->GetLogicalDevice(), &memAlloc, nullptr, &m_depthStencil.mem));
		VK_CHECK_RESULT(vkBindImageMemory(m_device->GetLogicalDevice(), m_depthStencil.image, m_depthStencil.mem, 0));

		VkImageSubresourceRange imageSubresourceRange = VkFactory::ImageSubresourceRange(VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT);
		VkImageViewCreateInfo imageViewCreateInfo = VkFactory::ImageViewCreateInfo(m_device->GetSupportedDepthFormat(), m_depthStencil.image, imageSubresourceRange);
		VK_CHECK_RESULT(vkCreateImageView(m_device->GetLogicalDevice(), &imageViewCreateInfo, nullptr, &m_depthStencil.view));
	}
	
	void Renderer::InitFramebuffers()
	{
		m_framebuffers.resize(m_swapchain->GetImageCount());
		for (UInt32 i = 0; i < static_cast<UInt32>(m_framebuffers.size()); i++)
		{
			std::vector<VkImageView> attachments(2);										
			attachments[0] = m_swapchain->GetBuffer(i).view;
			attachments[1] = m_depthStencil.view;									

			VkFramebufferCreateInfo frameBufferCreateInfo = VkFactory::FramebufferCreateInfo(m_renderPass, attachments, m_width, m_height);
			VK_CHECK_RESULT(vkCreateFramebuffer(m_device->GetLogicalDevice(), &frameBufferCreateInfo, nullptr, &m_framebuffers[i]));
		}
	}
	
	void Renderer::InitRenderPass()
	{
		std::vector<VkAttachmentDescription> attachments;
		// Color attachment
		attachments.push_back(VkFactory::AttachmentDescription(m_swapchain->GetColorFormat()));
		attachments[0].storeOp = VK_ATTACHMENT_STORE_OP_STORE;							
		attachments[0].finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;					
																						
		// Depth attachment
		attachments.push_back(VkFactory::AttachmentDescription(m_device->GetSupportedDepthFormat()));
		attachments[1].finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;	

		// Setup attachment references
		VkAttachmentReference colorReference = VkFactory::AttachmentReference(0, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);			
		VkAttachmentReference depthReference = VkFactory::AttachmentReference(1, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);	

		VkSubpassDescription subpassDescription = VkFactory::SubpassDescription(VK_PIPELINE_BIND_POINT_GRAPHICS);
		subpassDescription.pColorAttachments = &colorReference;							
		subpassDescription.pDepthStencilAttachment = &depthReference;					
		
		std::vector<VkSubpassDependency> dependencies;
		VkFactory::SubpassDependencies(dependencies);

		VkRenderPassCreateInfo renderPassInfo = VkFactory::RenderPassCreateInfo(attachments, dependencies, &subpassDescription);
		VK_CHECK_RESULT(vkCreateRenderPass(m_device->GetLogicalDevice(), &renderPassInfo, nullptr, &m_renderPass));
	}
	
	void Renderer::InitPipelineCache()
	{
		VkPipelineCacheCreateInfo pipelineCacheCreateInfo = VkFactory::PipelineCacheCreateInfo();
		VK_CHECK_RESULT(vkCreatePipelineCache(m_device->GetLogicalDevice(), &pipelineCacheCreateInfo, nullptr, &m_pipelineCache))
	}
	
	void Renderer::InitPipelines()
	{
		VkPipelineInputAssemblyStateCreateInfo inputAssemblyState = VkFactory::PipelineInputAssemblyStateCreateInfo();
		VkPipelineRasterizationStateCreateInfo rasterizationState = VkFactory::PipelineRasterizationStateCreateInfo();
		
		VkPipelineColorBlendAttachmentState blendAttachmentState = VkFactory::PipelineColorBlendAttachmentState();
		std::vector<VkPipelineColorBlendAttachmentState> colorBlendStateAttachments;
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
		VkFactory::VertexInputBindingDescriptions(vertexInputBindingDecriptions);
		
		std::vector<VkVertexInputAttributeDescription> vertexInputAttributeDescriptions;
		VkFactory::VertexInputAttributeDescriptions(vertexInputAttributeDescriptions);

		VkPipelineVertexInputStateCreateInfo vertexInputState = VkFactory::PipelineVertexInputStateCreateInfo(vertexInputBindingDecriptions, vertexInputAttributeDescriptions);

		std::vector<VkPipelineShaderStageCreateInfo> shaderStages;
		Shader vertexShader(m_device, ASSETS_DIR + std::string("shaders/triangle/triangle.vert.spv"));
		Shader fragmentShader(m_device, ASSETS_DIR + std::string("shaders/triangle/triangle.frag.spv"));
		shaderStages.push_back(VkFactory::PipelineShaderStageCreateInfo(VK_SHADER_STAGE_VERTEX_BIT, vertexShader.GetModule()));
		shaderStages.push_back(VkFactory::PipelineShaderStageCreateInfo(VK_SHADER_STAGE_FRAGMENT_BIT, fragmentShader.GetModule()));

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
		
		VK_CHECK_RESULT(vkCreateGraphicsPipelines(m_device->GetLogicalDevice(), m_pipelineCache, 1, &graphicsPipelineCreateInfo, nullptr, &m_pipeline));
	}
	
	void Renderer::InitConsole(std::string title)
	{
		AllocConsole();
		AttachConsole(GetCurrentProcessId());
		FILE *stream;
		freopen_s(&stream, "CONOUT$", "w+", stdout);
		freopen_s(&stream, "CONOUT$", "w+", stderr);

		SetConsoleTitle(TEXT(title.c_str()));
	}

	HWND Renderer::BuildWindow(HINSTANCE hinstance, WNDPROC wndproc, std::string name, std::string windowTitle)
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

			if ((m_width != (UInt32)screenWidth) && (m_height != (UInt32)screenHeight))
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
		windowRect.right = m_settings.fullscreen ? (long)screenWidth : (long)m_width;
		windowRect.bottom = m_settings.fullscreen ? (long)screenHeight : (long)m_height;

		AdjustWindowRectEx(&windowRect, dwStyle, FALSE, dwExStyle);

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
			UInt32 x = (GetSystemMetrics(SM_CXSCREEN) - windowRect.right) / 2;
			UInt32 y = (GetSystemMetrics(SM_CYSCREEN) - windowRect.bottom) / 2;
			SetWindowPos(m_window, 0, x, y, 0, 0, SWP_NOZORDER | SWP_NOSIZE);
		}

		if (!m_window)
		{
			printf("Could not create window!\n");
			fflush(stdout);
			return nullptr;
			exit(1);
		}

		ShowWindow(m_window, SW_SHOW);
		SetForegroundWindow(m_window);
		SetFocus(m_window);

		return m_window;
	}

	void Renderer::HandleMouseMove(Int32 x, Int32 y)
	{
		Int32 dx = (Int32)m_mousePos.x - x;
		Int32 dy = (Int32)m_mousePos.y - y;

		bool handled = false;

		if (m_settings.overlay) 
		{
			/* TODO
			ImGuiIO& io = ImGui::GetIO();
			handled = io.WantCaptureMouse;
			*/

		}
		MouseMoved((float)x, (float)y, handled);

		if (handled) 
		{
			m_mousePos = glm::vec2((float)x, (float)y);
			return;
		}

		if (m_mouseButtons.left) 
		{
			m_rotation.x += dy * 1.25f * m_rotationSpeed;
			m_rotation.y -= dx * 1.25f * m_rotationSpeed;
			m_camera.rotate(glm::vec3(dy * m_camera.rotationSpeed, -dx * m_camera.rotationSpeed, 0.0f));
			m_viewUpdated = true;
		}
		if (m_mouseButtons.right) 
		{
			m_zoom += dy * .005f * m_zoomSpeed;
			m_camera.translate(glm::vec3(-0.0f, 0.0f, dy * .005f * m_zoomSpeed));
			m_viewUpdated = true;
		}
		if (m_mouseButtons.middle) 
		{
			m_cameraPos.x -= dx * 0.01f;
			m_cameraPos.y -= dy * 0.01f;
			m_camera.translate(glm::vec3(-dx * 0.01f, -dy * 0.01f, 0.0f));
			m_viewUpdated = true;
		}
		m_mousePos = glm::vec2((float)x, (float)y);
	}

	void Renderer::WindowResize()
	{
		if (!m_prepared)
		{
			return;
		}
		m_prepared = false;

		vkDeviceWaitIdle(m_device->GetLogicalDevice());

		m_width = m_destWidth;
		m_height = m_destHeight;

		m_swapchain->CreateSwapchain(&m_width, &m_height, m_settings.vsync);

		vkDestroyImageView(m_device->GetLogicalDevice(), m_depthStencil.view, nullptr);
		vkDestroyImage(m_device->GetLogicalDevice(), m_depthStencil.image, nullptr);
		vkFreeMemory(m_device->GetLogicalDevice(), m_depthStencil.mem, nullptr);

		InitDepthStencil();
		for (UInt32 i = 0; i < m_framebuffers.size(); ++i) 
		{
			vkDestroyFramebuffer(m_device->GetLogicalDevice(), m_framebuffers[i], nullptr);
		}
		InitFramebuffers();

		// Command buffers need to be recreated as they may store
		// references to the recreated frame buffer
		DestroyCommandBuffers();
		CreateCommandBuffers();
		BuildCommandBuffers();

		vkDeviceWaitIdle(m_device->GetLogicalDevice());

		if (m_settings.overlay) 
		{
			/*
				UIOverlay->resize(width, height, frameBuffers);
			*/
		}

		m_camera.updateAspectRatio((float)m_width / (float)m_height);

		WindowResized();
		ViewChanged();

		m_prepared = true;
	}

	void Renderer::WindowResized()
	{
		m_uniforms->UpdateVS();
	}

	void Renderer::ViewChanged()
	{
		m_uniforms->UpdateVS();
	}

	void Renderer::HandleMessages(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		switch (uMsg)
		{
		case WM_CLOSE:
			m_prepared = false;
			DestroyWindow(hWnd);
			PostQuitMessage(0);
			break;
		case WM_PAINT:
			ValidateRect(m_window, NULL);
			break;
		case WM_KEYDOWN:
			switch (wParam)
			{
			case KEY_P:
				m_paused = !m_paused;
				break;
			case KEY_F1:
				if (m_settings.overlay) 
				{
					/** TODO */
				}
				break;
			case KEY_ESCAPE:
				PostQuitMessage(0);
				break;
			}

			if (m_camera.firstperson)
			{
				switch (wParam)
				{
				case KEY_W:
					m_camera.keys.up = true;
					break;
				case KEY_S:
					m_camera.keys.down = true;
					break;
				case KEY_A:
					m_camera.keys.left = true;
					break;
				case KEY_D:
					m_camera.keys.right = true;
					break;
				}
			}

			KeyPressed((UInt32)wParam);
			break;
		case WM_KEYUP:
			if (m_camera.firstperson)
			{
				switch (wParam)
				{
				case KEY_W:
					m_camera.keys.up = false;
					break;
				case KEY_S:
					m_camera.keys.down = false;
					break;
				case KEY_A:
					m_camera.keys.left = false;
					break;
				case KEY_D:
					m_camera.keys.right = false;
					break;
				}
			}
			break;
		case WM_LBUTTONDOWN:
			m_mousePos = glm::vec2((float)LOWORD(lParam), (float)HIWORD(lParam));
			m_mouseButtons.left = true;
			break;
		case WM_RBUTTONDOWN:
			m_mousePos = glm::vec2((float)LOWORD(lParam), (float)HIWORD(lParam));
			m_mouseButtons.right = true;
			break;
		case WM_MBUTTONDOWN:
			m_mousePos = glm::vec2((float)LOWORD(lParam), (float)HIWORD(lParam));
			m_mouseButtons.middle = true;
			break;
		case WM_LBUTTONUP:
			m_mouseButtons.left = false;
			break;
		case WM_RBUTTONUP:
			m_mouseButtons.right = false;
			break;
		case WM_MBUTTONUP:
			m_mouseButtons.middle = false;
			break;
		case WM_MOUSEWHEEL:
		{
			short wheelDelta = GET_WHEEL_DELTA_WPARAM(wParam);
			m_zoom += (float)wheelDelta * 0.005f * m_zoomSpeed;
			m_camera.translate(glm::vec3(0.0f, 0.0f, (float)wheelDelta * 0.005f * m_zoomSpeed));
			m_viewUpdated = true;
			break;
		}
		case WM_MOUSEMOVE:
		{
			HandleMouseMove(LOWORD(lParam), HIWORD(lParam));
			break;
		}
		case WM_SIZE:
			if ((m_prepared) && (wParam != SIZE_MINIMIZED))
			{
				if ((m_resizing) || ((wParam == SIZE_MAXIMIZED) || (wParam == SIZE_RESTORED)))
				{
					m_destWidth = LOWORD(lParam);
					m_destHeight = HIWORD(lParam);
					WindowResize();
				}
			}
			break;
		case WM_ENTERSIZEMOVE:
			m_resizing = true;
			break;
		case WM_EXITSIZEMOVE:
			m_resizing = false;
			break;
		}
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

		vkDeviceWaitIdle(m_device->GetLogicalDevice());
	}
	
	void Renderer::SubmitVerticesData(std::vector<Vertex> & verticesData, std::vector<UInt32> & indicesData)
	{
		m_vertices->Submit(verticesData, indicesData);
	}
	
	void Renderer::SetViewports(std::vector<VkViewport> & viewports)
	{
		m_viewports = std::move(viewports);
	}
	
	void Renderer::SetPipelines(std::vector<VkPipeline> & pipelines)
	{
		m_pipelines = std::move(pipelines);
		assert(m_pipelines.size() == m_viewports.size());
	}
	
	void Renderer::RenderFrame()
	{
		if (m_viewUpdated)
		{
			m_viewUpdated = false;
			ViewChanged();
		}

		VK_CHECK_RESULT(m_swapchain->AcquireNextImage(m_presentCompleteSemaphore, &m_currentBuffer));

		VK_CHECK_RESULT(vkWaitForFences(m_device->GetLogicalDevice(), 1, &m_fences[m_currentBuffer], VK_TRUE, UINT64_MAX));
		VK_CHECK_RESULT(vkResetFences(m_device->GetLogicalDevice(), 1, &m_fences[m_currentBuffer]));

		VkPipelineStageFlags waitStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		
		VkSubmitInfo submitInfo = VkFactory::SubmitInfo();
		submitInfo.pWaitDstStageMask = &waitStageMask;									
		submitInfo.pWaitSemaphores = &m_presentCompleteSemaphore;							
		submitInfo.waitSemaphoreCount = 1;																														
		submitInfo.pSignalSemaphores = &m_renderCompleteSemaphore;						
		submitInfo.signalSemaphoreCount = 1;											
		submitInfo.pCommandBuffers = &m_drawCmdBuffers[m_currentBuffer];					
		submitInfo.commandBufferCount = 1;												

		VK_CHECK_RESULT(vkQueueSubmit(m_device->GetQueue(), 1, &submitInfo, m_fences[m_currentBuffer]));
		
		VK_CHECK_RESULT(m_swapchain->QueuePresent(m_device->GetQueue(), m_currentBuffer, m_renderCompleteSemaphore));

		VK_CHECK_RESULT(vkQueueWaitIdle(m_device->GetQueue()));
	}
}