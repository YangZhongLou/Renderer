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
		vkDestroySemaphore(m_device->GetLogicalDevice(), renderCompleteSemaphore, nullptr);

		for (auto& fence : m_fences)
		{
			vkDestroyFence(m_device->GetLogicalDevice(), fence, nullptr);
		}
	}
	
	void Renderer::Init()
	{
		InitSync();
		InitVulkanInstance(true);
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
		return vkCreateInstance(&instanceCreateInfo, nullptr, &m_instance);
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
		std::array<VkAttachmentDescription, 2> attachments = {};

		// Color attachment
		attachments[0].format = m_swapchain->GetColorFormat();							
		attachments[0].samples = VK_SAMPLE_COUNT_1_BIT;									
		attachments[0].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;							
		attachments[0].storeOp = VK_ATTACHMENT_STORE_OP_STORE;							
		attachments[0].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;					
		attachments[0].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;				
		attachments[0].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;						
		attachments[0].finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;					
																						
		// Depth attachment
		attachments[1].format = m_depthFormat;											//
		attachments[1].samples = VK_SAMPLE_COUNT_1_BIT;						
		attachments[1].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;							// Clear depth at start of first subpass
		attachments[1].storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;						// We don't need depth after render pass has finished (DONT_CARE may result in better performance)
		attachments[1].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;					// No stencil
		attachments[1].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;				// No Stencil
		attachments[1].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;						// Layout at render pass start. Initial doesn't matter, so we use undefined
		attachments[1].finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;	// Transition to depth/stencil attachment

		// Setup attachment references
		VkAttachmentReference colorReference = {};
		colorReference.attachment = 0;													// Attachment 0 is color
		colorReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;				// Attachment layout used as color during the subpass

		VkAttachmentReference depthReference = {};
		depthReference.attachment = 1;													// Attachment 1 is color
		depthReference.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;		// Attachment used as depth/stemcil used during the subpass

		// Setup a single subpass reference
		VkSubpassDescription subpassDescription = {};
		subpassDescription.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;			
		subpassDescription.colorAttachmentCount = 1;									// Subpass uses one color attachment
		subpassDescription.pColorAttachments = &colorReference;							// Reference to the color attachment in slot 0
		subpassDescription.pDepthStencilAttachment = &depthReference;					// Reference to the depth attachment in slot 1
		subpassDescription.inputAttachmentCount = 0;									// Input attachments can be used to sample from contents of a previous subpass
		subpassDescription.pInputAttachments = nullptr;									// (Input attachments not used by this example)
		subpassDescription.preserveAttachmentCount = 0;									// Preserved attachments can be used to loop (and preserve) attachments through subpasses
		subpassDescription.pPreserveAttachments = nullptr;								// (Preserve attachments not used by this example)
		subpassDescription.pResolveAttachments = nullptr;								// Resolve attachments are resolved at the end of a sub pass and can be used for e.g. multi sampling

		// Setup subpass dependencies
		// These will add the implicit attachment layout transitions specified by the attachment descriptions
		// The actual usage layout is preserved through the layout specified in the attachment reference		
		// Each subpass dependency will introduce a memory and execution dependency between the source and dest subpass described by
		// srcStageMask, dstStageMask, srcAccessMask, dstAccessMask (and dependencyFlags is set)
		// Note: VK_SUBPASS_EXTERNAL is a special constant that refers to all commands executed outside of the actual renderpass)
		std::array<VkSubpassDependency, 2> dependencies;

		// First dependency at the start of the renderpass
		// Does the transition from final to initial layout 
		dependencies[0].srcSubpass = VK_SUBPASS_EXTERNAL;								// Producer of the dependency 
		dependencies[0].dstSubpass = 0;													// Consumer is our single subpass that will wait for the execution depdendency
		dependencies[0].srcStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;			
		dependencies[0].dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;	
		dependencies[0].srcAccessMask = VK_ACCESS_MEMORY_READ_BIT;
		dependencies[0].dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
		dependencies[0].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

		// Second dependency at the end the renderpass
		// Does the transition from the initial to the final layout
		dependencies[1].srcSubpass = 0;													// Producer of the dependency is our single subpass
		dependencies[1].dstSubpass = VK_SUBPASS_EXTERNAL;								// Consumer are all commands outside of the renderpass
		dependencies[1].srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;	
		dependencies[1].dstStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
		dependencies[1].srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
		dependencies[1].dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;
		dependencies[1].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

		// Create the actual renderpass
		VkRenderPassCreateInfo renderPassInfo = {};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());		// Number of attachments used by this render pass
		renderPassInfo.pAttachments = attachments.data();								// Descriptions of the attachments used by the render pass
		renderPassInfo.subpassCount = 1;												// We only use one subpass in this example
		renderPassInfo.pSubpasses = &subpassDescription;								// Description of that subpass
		renderPassInfo.dependencyCount = static_cast<uint32_t>(dependencies.size());	// Number of subpass dependencies
		renderPassInfo.pDependencies = dependencies.data();								// Subpass dependencies used by the render pass

		VK_CHECK_RESULT(vkCreateRenderPass(device, &renderPassInfo, nullptr, &renderPass));
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
}