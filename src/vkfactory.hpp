/*
* This code is licensed under the MIT license (MIT) (http://opensource.org/licenses/MIT)
*/
#pragma once

#include <stdlib.h>
#include <vector>
#include <vulkan/vulkan.h>
#include "types.h"
/** refactor */
#include "vertices.h"


namespace Concise
{
	struct Vertex;

	namespace VkFactory
	{	
		inline VkPresentInfoKHR PresentInfoKHR(VkSwapchainKHR * swapchain, UInt32 imageIndex)
		{
			VkPresentInfoKHR presentInfo = {};
			presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
			presentInfo.pNext = NULL;
			presentInfo.swapchainCount = 1;
			presentInfo.pSwapchains = swapchain;
			presentInfo.pImageIndices = &imageIndex;
			
			return presentInfo;
		}
	
		inline VkCommandPoolCreateInfo CommandPoolCreateInfo(UInt32 queueFamilyIndex, VkCommandPoolCreateFlags flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT)
		{
			VkCommandPoolCreateInfo commandPoolCreateInfo = {};
			commandPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
			commandPoolCreateInfo.queueFamilyIndex = queueFamilyIndex;
			commandPoolCreateInfo.flags = flags;
			
			return commandPoolCreateInfo;
		}
	
		inline VkDeviceCreateInfo DeviceCreateInfo(std::vector<VkDeviceQueueCreateInfo> & queueCreateInfos, VkPhysicalDeviceFeatures enabledFeatures)
		{
			VkDeviceCreateInfo deviceCreateInfo = {};
			deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
			deviceCreateInfo.queueCreateInfoCount = static_cast<UInt32>(queueCreateInfos.size());;
			deviceCreateInfo.pQueueCreateInfos = queueCreateInfos.data();
			deviceCreateInfo.pEnabledFeatures = &enabledFeatures;
			
			return deviceCreateInfo;
		}
	
		inline VkDeviceQueueCreateInfo DeviceQueueCreateInfo(UInt32 queueFamilyIndex,  const float * queuePriorities)
		{
			VkDeviceQueueCreateInfo deviceQueueCreateInfo{};
			deviceQueueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
			deviceQueueCreateInfo.queueFamilyIndex = queueFamilyIndex;
			deviceQueueCreateInfo.queueCount = 1;
			deviceQueueCreateInfo.pQueuePriorities = queuePriorities;
			
			return deviceQueueCreateInfo;
		}
	
		inline VkDebugReportCallbackCreateInfoEXT DebugReportCallbackCreateInfo(PFN_vkDebugReportCallbackEXT pfnCallback, VkDebugReportFlagsEXT flags)
		{
			VkDebugReportCallbackCreateInfoEXT debugReportCallbackCreateInfo = {};
			debugReportCallbackCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CREATE_INFO_EXT;
			debugReportCallbackCreateInfo.pfnCallback = pfnCallback;
			debugReportCallbackCreateInfo.flags = flags;
			
			return debugReportCallbackCreateInfo;
		}
		
		inline VkSwapchainCreateInfoKHR SwapchainCreateInfoKHR()
		{
			VkSwapchainCreateInfoKHR swapchainCreateInfo = {};
			swapchainCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
			swapchainCreateInfo.pNext = NULL;
			
			return swapchainCreateInfo;
		}
	
		inline VkWin32SurfaceCreateInfoKHR Win32SurfaceCreateInfoKHR()
		{
			VkWin32SurfaceCreateInfoKHR surfaceCreateInfo = {};
			surfaceCreateInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
			
			return surfaceCreateInfo;
		}
	
		inline VkPipelineCacheCreateInfo PipelineCacheCreateInfo()
		{
			VkPipelineCacheCreateInfo pipelineCacheCreateInfo = {};
			pipelineCacheCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO;
			return pipelineCacheCreateInfo;
		}
	
		inline VkPipelineShaderStageCreateInfo PipelineShaderStageCreateInfo(VkShaderStageFlagBits stage, VkShaderModule & module)
		{
			VkPipelineShaderStageCreateInfo shaderStageCreateInfo;
			shaderStageCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
			shaderStageCreateInfo.stage = stage;
			shaderStageCreateInfo.module = module;
			shaderStageCreateInfo.pName = "main";
			
			return shaderStageCreateInfo;
		}
	
		inline VkPipelineVertexInputStateCreateInfo PipelineVertexInputStateCreateInfo(std::vector<VkVertexInputBindingDescription> & vertexBindingDescriptions, 
			std::vector<VkVertexInputAttributeDescription> & vertexAttributeDescriptions)
		{
			VkPipelineVertexInputStateCreateInfo vertexInputState = {};
			vertexInputState.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
			vertexInputState.vertexBindingDescriptionCount = static_cast<UInt32>(vertexBindingDescriptions.size());
			vertexInputState.pVertexBindingDescriptions = vertexBindingDescriptions.data();
			vertexInputState.vertexAttributeDescriptionCount = static_cast<UInt32>(vertexAttributeDescriptions.size());
			vertexInputState.pVertexAttributeDescriptions = vertexAttributeDescriptions.data();

			return vertexInputState;
		}
		
		inline void VertexInputBindingDescriptions(std::vector<VkVertexInputBindingDescription> & vertexBindingDescriptions)
		{
			VkVertexInputBindingDescription vertexInputBinding;
			vertexInputBinding.binding = 0;
			vertexInputBinding.stride = sizeof(Vertex);
			vertexInputBinding.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
			
			vertexBindingDescriptions.push_back(vertexInputBinding);
		}
	
		inline void VertexInputAttributeDescription(std::vector<VkVertexInputAttributeDescription> & vertexInputAttributeDescriptions)
		{
			VkVertexInputAttributeDescription vertexInputAttributeDescription {};
			
			vertexInputAttributeDescription.binding = 0;
			vertexInputAttributeDescription.location = 0;
			vertexInputAttributeDescription.format = VK_FORMAT_R32G32B32_SFLOAT;
			vertexInputAttributeDescription.offset = offsetof(Vertex, positions);
			vertexInputAttributeDescriptions.push_back(vertexInputAttributeDescription);
			
			vertexInputAttributeDescription.binding = 0;
			vertexInputAttributeDescription.location = 1;
			vertexInputAttributeDescription.format = VK_FORMAT_R32G32B32_SFLOAT;
			vertexInputAttributeDescription.offset = offsetof(Vertex, colors);
			vertexInputAttributeDescriptions.push_back(vertexInputAttributeDescription);
		}
	
		inline VkPipelineMultisampleStateCreateInfo PipelineMultisampleStateCreateInfo()
		{
			VkPipelineMultisampleStateCreateInfo multisampleState = {};
			multisampleState.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
			multisampleState.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
			multisampleState.pSampleMask = nullptr;
			
			return multisampleState;
		}
	
		inline VkPipelineDepthStencilStateCreateInfo PipelineDepthStencilStateCreateInfo()
		{
			VkPipelineDepthStencilStateCreateInfo depthStencilState = {};
			depthStencilState.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
			depthStencilState.depthTestEnable = VK_TRUE;
			depthStencilState.depthWriteEnable = VK_TRUE;
			depthStencilState.depthCompareOp = VK_COMPARE_OP_LESS_OR_EQUAL;
			depthStencilState.depthBoundsTestEnable = VK_FALSE;
			depthStencilState.back.failOp = VK_STENCIL_OP_KEEP;
			depthStencilState.back.passOp = VK_STENCIL_OP_KEEP;
			depthStencilState.back.compareOp = VK_COMPARE_OP_ALWAYS;
			depthStencilState.stencilTestEnable = VK_FALSE;
			depthStencilState.front = depthStencilState.back;
			
			return depthStencilState;
		}
	
		inline VkPipelineDynamicStateCreateInfo PipelineDynamicStateCreateInfo(std::vector<VkDynamicState> & dynamicStates)
		{
			VkPipelineDynamicStateCreateInfo dynamicState = {};
			dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
			dynamicState.pDynamicStates = dynamicStates.data();
			dynamicState.dynamicStateCount = static_cast<UInt32>(dynamicStates.size());
			
			return dynamicState;
		}
		
		inline VkPipelineViewportStateCreateInfo PipelineViewportStateCreateInfo()
		{
			VkPipelineViewportStateCreateInfo viewportState = {};
			viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
			viewportState.viewportCount = 1;
			viewportState.scissorCount = 1;
			
			return viewportState;
		}

		inline VkPipelineColorBlendStateCreateInfo PipelineColorBlendStateCreateInfo(std::vector<VkPipelineColorBlendAttachmentState> & attachments)
		{
			VkPipelineColorBlendStateCreateInfo colorBlendState = {};
			colorBlendState.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
			colorBlendState.attachmentCount = static_cast<UInt32>(attachments.size());
			colorBlendState.pAttachments = attachments.data();
			
			return colorBlendState;
		}

		inline VkPipelineColorBlendAttachmentState PipelineColorBlendAttachmentState()
		{
			VkPipelineColorBlendAttachmentState blendAttachmentState = {};
			blendAttachmentState.colorWriteMask = 0xf;
			blendAttachmentState.blendEnable = VK_FALSE;
			
			return blendAttachmentState;
		}
		
		inline VkPipelineRasterizationStateCreateInfo PipelineRasterizationStateCreateInfo()
		{
			VkPipelineRasterizationStateCreateInfo rasterizationState;
			rasterizationState.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
			rasterizationState.polygonMode = VK_POLYGON_MODE_FILL;
			rasterizationState.cullMode = VK_CULL_MODE_NONE;
			rasterizationState.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
			rasterizationState.depthClampEnable = VK_FALSE;
			rasterizationState.rasterizerDiscardEnable = VK_FALSE;
			rasterizationState.depthBiasEnable = VK_FALSE;
			rasterizationState.lineWidth = 1.0f;
			
			return rasterizationState;
		}
	
		inline VkPipelineInputAssemblyStateCreateInfo PipelineInputAssemblyStateCreateInfo()
		{
			VkPipelineInputAssemblyStateCreateInfo inputAssemblyState;
			inputAssemblyState.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
			inputAssemblyState.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
			
			return inputAssemblyState;
		}
		
		inline VkGraphicsPipelineCreateInfo GraphicsPipelineCreateInfo(
			std::vector<VkPipelineShaderStageCreateInfo> & shaderStages,
			VkPipelineVertexInputStateCreateInfo * vertexInputState,
			VkPipelineInputAssemblyStateCreateInfo * inputAssemblyState,
			VkPipelineViewportStateCreateInfo * viewportState,
			VkPipelineRasterizationStateCreateInfo * rasterizationState,
			VkPipelineMultisampleStateCreateInfo * multisampleState,
			VkPipelineDepthStencilStateCreateInfo * depthStencilState,
			VkPipelineColorBlendStateCreateInfo * colorBlendState,
			VkPipelineDynamicStateCreateInfo * dynamicState,
			VkPipelineLayout & layout,
			VkRenderPass & renderPass,
			VkPipelineTessellationStateCreateInfo * tessellationState = nullptr)
		{
			VkGraphicsPipelineCreateInfo graphicsPipelineCreateInfo;
			graphicsPipelineCreateInfo.flags = VK_PIPELINE_CREATE_DISABLE_OPTIMIZATION_BIT;
			graphicsPipelineCreateInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
			graphicsPipelineCreateInfo.layout = layout;
			graphicsPipelineCreateInfo.stageCount = static_cast<UInt32>(shaderStages.size());
			graphicsPipelineCreateInfo.pStages = shaderStages.data();

			graphicsPipelineCreateInfo.pVertexInputState = vertexInputState;
			graphicsPipelineCreateInfo.pInputAssemblyState = inputAssemblyState;
			graphicsPipelineCreateInfo.pRasterizationState = rasterizationState;
			graphicsPipelineCreateInfo.pColorBlendState = colorBlendState;
			graphicsPipelineCreateInfo.pMultisampleState = multisampleState;
			graphicsPipelineCreateInfo.pViewportState = viewportState;
			graphicsPipelineCreateInfo.pDepthStencilState = depthStencilState;
			graphicsPipelineCreateInfo.renderPass = renderPass;
			graphicsPipelineCreateInfo.pDynamicState = dynamicState;
			
			return graphicsPipelineCreateInfo;
		}
	
		inline VkShaderModuleCreateInfo ShaderModuleCreateInfo(UInt32 * code, Size codeSize)
		{
			VkShaderModuleCreateInfo shaderModuleCreateInfo{};
			shaderModuleCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
			shaderModuleCreateInfo.codeSize = codeSize;
			shaderModuleCreateInfo.pCode = code;
			
			return shaderModuleCreateInfo;
		}
		
		inline void SubpassDependencies(std::vector<VkSubpassDependency> & subpassDependencies)
		{
			VkSubpassDependency subpassDependency {};
			subpassDependency.srcSubpass = VK_SUBPASS_EXTERNAL;								
			subpassDependency.dstSubpass = 0;													
			subpassDependency.srcStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;			
			subpassDependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;	
			subpassDependency.srcAccessMask = VK_ACCESS_MEMORY_READ_BIT;
			subpassDependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
			subpassDependency.dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;
			subpassDependencies.push_back(subpassDependency);

			subpassDependency.srcSubpass = 0;													
			subpassDependency.dstSubpass = VK_SUBPASS_EXTERNAL;							
			subpassDependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;	
			subpassDependency.dstStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
			subpassDependency.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
			subpassDependency.dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;
			subpassDependency.dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;
			subpassDependencies.push_back(subpassDependency);
		}
	
		inline VkRenderPassCreateInfo RenderPassCreateInfo(std::vector<VkAttachmentDescription> & attachments, std::vector<VkSubpassDependency> & dependencies, VkSubpassDescription * subpassDescription)
		{
			VkRenderPassCreateInfo renderPassInfo = {};
			renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
			renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());		
			renderPassInfo.pAttachments = attachments.data();							
			renderPassInfo.subpassCount = 1;												
			renderPassInfo.pSubpasses = subpassDescription;								
			renderPassInfo.dependencyCount = static_cast<uint32_t>(dependencies.size());	
			renderPassInfo.pDependencies = dependencies.data();			

			return renderPassInfo;
		}
	
		inline VkSubpassDescription SubpassDescription(VkPipelineBindPoint pipelineBindPoint)
		{
			VkSubpassDescription subpassDescription;
			subpassDescription.flags = VK_SUBPASS_DESCRIPTION_PER_VIEW_ATTRIBUTES_BIT_NVX;
			subpassDescription.pipelineBindPoint = pipelineBindPoint;			
			subpassDescription.colorAttachmentCount = 1;									
			subpassDescription.pColorAttachments = nullptr;							
			subpassDescription.pDepthStencilAttachment = nullptr;					
			subpassDescription.inputAttachmentCount = 0;									
			subpassDescription.pInputAttachments = nullptr;									
			subpassDescription.preserveAttachmentCount = 0;									
			subpassDescription.pPreserveAttachments = nullptr;								
			subpassDescription.pResolveAttachments = nullptr;
			
			return subpassDescription;
		}
	
		inline VkAttachmentReference AttachmentReference(UInt32 attachment, VkImageLayout layout)
		{
			VkAttachmentReference attachmentReference;
			attachmentReference.attachment = attachment;													
			attachmentReference.layout = layout;
			
			return attachmentReference;
		}
		
		inline VkAttachmentDescription AttachmentDescription(VkFormat format)
		{
			VkAttachmentDescription attachmentDescription;
			attachmentDescription.flags = VK_ATTACHMENT_DESCRIPTION_MAY_ALIAS_BIT;
			attachmentDescription.format = format;							
			attachmentDescription.samples = VK_SAMPLE_COUNT_1_BIT;									
			attachmentDescription.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;							
			attachmentDescription.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;							
			attachmentDescription.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;					
			attachmentDescription.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;				
			attachmentDescription.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;						
			attachmentDescription.finalLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			
			return attachmentDescription;
		}
	
		inline VkFramebufferCreateInfo FramebufferCreateInfo(VkRenderPass renderPass, std::vector<VkImageView>& attachments,
			UInt32 width, UInt32 height)
		{
			VkFramebufferCreateInfo frameBufferCreateInfo = {};
			frameBufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
			frameBufferCreateInfo.renderPass = renderPass;
			frameBufferCreateInfo.attachmentCount = static_cast<UInt32>(attachments.size());
			frameBufferCreateInfo.pAttachments = attachments.data();
			frameBufferCreateInfo.width = width;
			frameBufferCreateInfo.height = height;
			frameBufferCreateInfo.layers = 1;
			
			return frameBufferCreateInfo;
		}
	
		inline VkImageSubresourceRange ImageSubresourceRange(VkImageAspectFlags aspectMask, 
			UInt32 baseMipLevel = 0, UInt32 levelCount = 1, UInt32 baseArrayLayer = 0, UInt32 layerCount = 1)
		{
			VkImageSubresourceRange imageSubresourceRange = {};
			
			imageSubresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT;
			imageSubresourceRange.baseMipLevel = baseMipLevel;
			imageSubresourceRange.levelCount = levelCount;
			imageSubresourceRange.baseArrayLayer = baseArrayLayer;
			imageSubresourceRange.layerCount = layerCount;
			
			return imageSubresourceRange;
		}
	
		inline VkImageViewCreateInfo ImageViewCreateInfo(VkFormat format, VkImage image, VkImageSubresourceRange subresourceRange)
		{
			VkImageViewCreateInfo imageViewCreateInfo = {};
			imageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			imageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
			imageViewCreateInfo.format = format;
			imageViewCreateInfo.subresourceRange = subresourceRange;
			imageViewCreateInfo.image = image;
			
			return imageViewCreateInfo;
		}
		
		inline VkImageCreateInfo ImageCreateInfo(VkFormat format, VkExtent3D extent)
		{
			VkImageCreateInfo imageCreateInfo = {};
			imageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
			imageCreateInfo.imageType = VK_IMAGE_TYPE_2D;
			imageCreateInfo.format = format;
			imageCreateInfo.extent = extent;
			imageCreateInfo.mipLevels = 1;
			imageCreateInfo.arrayLayers = 1;
			imageCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;
			imageCreateInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
			imageCreateInfo.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
			imageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			
			return imageCreateInfo;
		}
	
		inline VkWriteDescriptorSet WriteDescriptorSet(VkDescriptorSet & dstSet, VkDescriptorType descriptorType, const VkDescriptorBufferInfo & pBufferInfo)
		{
			VkWriteDescriptorSet writeDescriptorSet = {};
			writeDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			writeDescriptorSet.dstSet = dstSet;
			writeDescriptorSet.descriptorCount = 1;
			writeDescriptorSet.descriptorType = descriptorType;
			writeDescriptorSet.pBufferInfo = &pBufferInfo;
			writeDescriptorSet.dstBinding = 0;
			
			return writeDescriptorSet;
		}
		
		inline VkDescriptorSetAllocateInfo DescriptorSetAllocateInfo(VkDescriptorSetLayout & descriptorSetLayout, VkDescriptorPool & descriptorPool)
		{
			VkDescriptorSetAllocateInfo descriptorSetAllocateInfo = {};
			descriptorSetAllocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
			descriptorSetAllocateInfo.descriptorPool = descriptorPool;
			descriptorSetAllocateInfo.descriptorSetCount = 1;
			descriptorSetAllocateInfo.pSetLayouts = &descriptorSetLayout;
			
			return descriptorSetAllocateInfo;
		}
		
		inline VkPipelineLayoutCreateInfo PipelineLayoutCreateInfo(VkDescriptorSetLayout & descriptorSetLayout)
		{
			VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo = {};
			pipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
			pipelineLayoutCreateInfo.pNext = nullptr;
			pipelineLayoutCreateInfo.setLayoutCount = 1;
			pipelineLayoutCreateInfo.pSetLayouts = &descriptorSetLayout;
			
			return pipelineLayoutCreateInfo;
		}

		inline VkDescriptorSetLayoutCreateInfo DescriptorSetLayoutCreateInfo(VkDescriptorSetLayoutBinding & layoutBinding)
		{
			VkDescriptorSetLayoutCreateInfo descriptorLayout = {};
			descriptorLayout.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
			descriptorLayout.pNext = nullptr;
			descriptorLayout.bindingCount = 1;
			descriptorLayout.pBindings = &layoutBinding;
			
			return descriptorLayout;
		}
		
		inline VkDescriptorSetLayoutBinding DescriptorSetLayoutBinding(VkDescriptorType descriptorType, VkShaderStageFlags stageFlags, UInt32 descriptorCount = 1)
		{
			VkDescriptorSetLayoutBinding layoutBinding = {};
			layoutBinding.descriptorType = descriptorType;
			layoutBinding.descriptorCount = descriptorCount;
			layoutBinding.stageFlags = stageFlags;
			layoutBinding.pImmutableSamplers = nullptr;
			
			return layoutBinding;
		}
		
		inline VkDescriptorPoolCreateInfo DescriptorPoolCreateInfo()
		{
			VkDescriptorPoolCreateInfo descriptorPoolInfo = {};
			descriptorPoolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
			descriptorPoolInfo.pNext = nullptr;
			
			return descriptorPoolInfo;
		}
		
		inline VkApplicationInfo ApplicationInfo()
		{
			VkApplicationInfo appInfo = {};
			appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
			appInfo.pApplicationName = "Concise";
			appInfo.pEngineName = "Concise";
			appInfo.apiVersion = VK_API_VERSION_1_0;
			
			return appInfo;
		}
		
		inline VkInstanceCreateInfo InstanceCreateInfo(VkApplicationInfo appInfo)
		{
			VkInstanceCreateInfo instanceCreateInfo = {};
			instanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
			instanceCreateInfo.pNext = NULL;
			instanceCreateInfo.pApplicationInfo = &appInfo;
			
			return instanceCreateInfo;
		}
		
		inline VkRect2D Scissor(Int32 width, Int32 height, Int32 offsetX = 0, Int32 offsetY = 0)
		{
			VkRect2D scissor = {};
			scissor.extent.width = width;
			scissor.extent.height = height;
			scissor.offset.x = offsetX;
			scissor.offset.y = offsetY;
			
			return scissor;
		}
		
		inline VkViewport ViewPort(float width, float height, float minDepth, float maxDepth)
		{
			VkViewport viewport = {};
			viewport.height = height;
			viewport.width = width;
			viewport.minDepth = 0.0f;
			viewport.maxDepth = 1.0f;
			
			return viewport;
		}
		
		inline VkRenderPassBeginInfo RenderPassBeginInfo()
		{
			VkRenderPassBeginInfo renderPassBeginInfo = {};
			renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
			renderPassBeginInfo.pNext = nullptr;
			
			return renderPassBeginInfo;
		}
		
		inline VkCommandBufferAllocateInfo CommandBufferAllocateInfo(VkCommandPool commandPool, VkCommandBufferLevel level = VK_COMMAND_BUFFER_LEVEL_PRIMARY, UInt32 commandBufferCount = 1)
		{
			VkCommandBufferAllocateInfo cmdBufAllocateInfo = {};
			cmdBufAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
			cmdBufAllocateInfo.commandPool = commandPool;
			cmdBufAllocateInfo.level = level;
			cmdBufAllocateInfo.commandBufferCount = commandBufferCount;
			
			return cmdBufAllocateInfo;
		}
		
		inline VkSemaphoreCreateInfo SemaphoreCreateInfo()
		{
			VkSemaphoreCreateInfo semaphoreCreateInfo {};
			semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
			semaphoreCreateInfo.pNext = nullptr;
			
			return semaphoreCreateInfo;
		}
		
		inline VkFenceCreateInfo FenceCreateInfo(VkFenceCreateFlags flags = 0)
		{
			VkFenceCreateInfo fenceCreateInfo = {};
			fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
			fenceCreateInfo.flags = flags;
			
			return fenceCreateInfo;
		}
		
		inline VkSubmitInfo SubmitInfo()
		{
			VkSubmitInfo submitInfo;
			submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

			return submitInfo;
		}
		
		inline VkSubmitInfo SubmitInfo(std::vector<VkCommandBuffer>& commandBuffers)
		{
			VkSubmitInfo submitInfo = {};
			submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
			submitInfo.commandBufferCount = static_cast<UInt32>(commandBuffers.size());
			submitInfo.pCommandBuffers = commandBuffers.data();
			
			return submitInfo;
		}
		
		inline VkMemoryAllocateInfo MemoryAllocateInfo(VkDeviceSize size, UInt32 memoryTypeIndex)
		{
			VkMemoryAllocateInfo memAlloc = {};
			memAlloc.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
			memAlloc.allocationSize = size;
			memAlloc.memoryTypeIndex = memoryTypeIndex;
			
			return memAlloc;
		}
		
		inline VkBufferCreateInfo BufferCreateInfo(VkDeviceSize size, VkBufferUsageFlags usage)
		{
			VkBufferCreateInfo bufferCreateInfo {};
			bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
			bufferCreateInfo.size = size;
			bufferCreateInfo.usage = usage;
			
			return bufferCreateInfo;
		}
		
		inline VkCommandBufferBeginInfo CommandBufferBeginInfo()
		{
			VkCommandBufferBeginInfo cmdBufferBeginInfo {};
			cmdBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
			
			return cmdBufferBeginInfo;
		}
	}
}