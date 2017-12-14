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
		vkDestroySemaphore(m_device->GetLogicalDevice(), m_presentCompleteSemaphore, nullptr);
		vkDestroySemaphore(m_device->GetLogicalDevice(), m_renderCompleteSemaphore, nullptr);

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
	
	void Renderer::InitVulkanSync()
	{
		VkSemaphoreCreateInfo semaphoreCreateInfo = VkFactory::SemaphoreCreateInfo();
		Utils::VK_CHECK_RESULT(vkCreateSemaphore(m_device->GetLogicalDevice(), &semaphoreCreateInfo, nullptr, &m_presentCompleteSemaphore));
		Utils::VK_CHECK_RESULT(vkCreateSemaphore(m_device->GetLogicalDevice(), &semaphoreCreateInfo, nullptr, &m_renderCompleteSemaphore));
		
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
	
	void InitSwapchain()
	{
		m_swapchain = new Swapchain(m_vkInstance, m_device);
		m_swapchain->Init();
		m_swapchain->CreateSwapchain(m_width, m_height, m_settings.vsync);
	}
	
	void Renderer::InitVulkan()
	{
		InitVulkanInstance(false);
		InitVulkanDebugger();
		InitVulkanDevice();
		InitSwapchain();
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
			instanceCreateInfo.enabledExtensionCount = (UInt32)instanceExtensions.size();
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
	
	void Renderer::CreateWindow(HINSTANCE hinstance, WNDPROC wndproc)
	{
		m_swapchain->CreateWindow(hinstance, wndproc)
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
		Utils::VK_CHECK_RESULT(m_swapchain->AcquireNextImage(m_presentCompleteSemaphore, &m_currentBuffer));

		Utils::VK_CHECK_RESULT(vkWaitForFences(m_device->GetLogicalDevice(), 1, &m_fences[m_currentBuffer], VK_TRUE, UINT64_MAX));
		Utils::VK_CHECK_RESULT(vkResetFences(m_device->GetLogicalDevice(), 1, &m_fences[m_currentBuffer]));

		VkPipelineStageFlags waitStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		
		VkSubmitInfo submitInfo = VkFactory::SubmitInfo();
		submitInfo.pWaitDstStageMask = &waitStageMask;									
		submitInfo.pWaitSemaphores = &m_presentCompleteSemaphore;							
		submitInfo.waitSemaphoreCount = 1;																														
		submitInfo.pSignalSemaphores = &m_renderCompleteSemaphore;						
		submitInfo.signalSemaphoreCount = 1;											
		submitInfo.pCommandBuffers = &m_drawCommandBuffers[m_currentBuffer];					
		submitInfo.commandBufferCount = 1;												

		Utils::VK_CHECK_RESULT(vkQueueSubmit(m_device->GetQueue(), 1, &submitInfo, m_fences[m_currentBuffer]));
		
		Utils::VK_CHECK_RESULT(m_swapchain->QueuePresent((m_device->GetQueue(), m_currentBuffer, m_renderCompleteSemaphore));
	}
}