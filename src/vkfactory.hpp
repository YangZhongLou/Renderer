/*
* This code is licensed under the MIT license (MIT) (http://opensource.org/licenses/MIT)
*/
#pragma once

#include <vector>
#include <vulkan/vulkan.h>
#include "types.h"

namespace Consice
{
	namespace VkFactory
	{	
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
	
		inline VkWriteDescriptorSet WriteDescriptorSet(VkDescriptorSet dstSet, VkDescriptorType descriptorType, const VkDescriptorBufferInfo* pBufferInfo)
		{
			VkWriteDescriptorSet writeDescriptorSet = {};
			writeDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			writeDescriptorSet.dstSet = dstSet;
			writeDescriptorSet.descriptorCount = 1;
			writeDescriptorSet.descriptorType = descriptorType;
			writeDescriptorSet.pBufferInfo = pBufferInfo;
			writeDescriptorSet.dstBinding = 0;
			
			return writeDescriptorSet;
		}
		
		inline VkDescriptorSetAllocateInfo DescriptorSetAllocateInfo(VkDescriptorSetLayout descriptorSetLayout)
		{
			VkDescriptorSetAllocateInfo descriptorSetAllocateInfo = {};
			descriptorSetAllocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
			descriptorSetAllocateInfo.descriptorPool = descriptorPool;
			descriptorSetAllocateInfo.descriptorSetCount = 1;
			descriptorSetAllocateInfo.pSetLayouts = &descriptorSetLayout;
			
			return descriptorSetAllocateInfo;
		}
		
		inline VkPipelineLayoutCreateInfo PipelineLayoutCreateInfo(VkDescriptorSetLayout descriptorSetLayout)
		{
			VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo = {};
			pipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
			pipelineLayoutCreateInfo.pNext = nullptr;
			pipelineLayoutCreateInfo.setLayoutCount = 1;
			pipelineLayoutCreateInfo.pSetLayouts = &descriptorSetLayout;
			
			return pipelineLayoutCreateInfo;
		}

		inline VkDescriptorSetLayoutCreateInfo DescriptorSetLayoutCreateInfo(const VkDescriptorSetLayoutBinding & layoutBinding)
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
		
		inline VkCommandBufferAllocateInfo CommandBufferAllocateInfo(VkCommandPool commandPool, VkCommandBufferLevel level)
		{
			VkCommandBufferAllocateInfo cmdBufAllocateInfo = {};
			cmdBufAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
			cmdBufAllocateInfo.commandPool = commandPool;
			cmdBufAllocateInfo.level = level;
			cmdBufAllocateInfo.commandBufferCount = 1;
			
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
		
		inline VkSubmitInfo SubmitInfo(std::vector<VkCommandBuffer>& commandBuffers)
		{
			VkSubmitInfo submitInfo = {};
			submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
			submitInfo.commandBufferCount = commandBuffers.size();
			submitInfo.pCommandBuffers = &commandBuffers.data();
			
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