#include "pipelines.h"
#include "device.h"
#include "defines.h"
#include "vk_factory.hpp"
#include "shader.h"
#include "shaders.h"
#include "renderpass.h"
#include "renderer.h"

namespace Concise
{
	Pipelines::Pipelines()
	{	
		VkPushConstantRange pushConstantRange =
			VkFactory::PushConstantRange(
				VK_SHADER_STAGE_VERTEX_BIT,
				sizeof(PushConstants),
				0);

		VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo =
			VkFactory::PipelineLayoutCreateInfo(
				1,
				&Shaders::Instance().GetDescriptorSetLayout(),
				1,
				&pushConstantRange
				);

		VK_CHECK_RESULT(vkCreatePipelineLayout(LogicalDevice, 
			&pipelineLayoutCreateInfo, nullptr, &m_pipelineLayout));

		VkPipelineCacheCreateInfo pipelineCacheCreateInfo = VkFactory::PipelineCacheCreateInfo();
		VK_CHECK_RESULT(vkCreatePipelineCache(LogicalDevice,
			&pipelineCacheCreateInfo, nullptr, &m_pipelineCache));


		VkPipelineInputAssemblyStateCreateInfo inputAssemblyState =
			VkFactory::PipelineInputAssemblyStateCreateInfo(
				0,
				VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
				VK_FALSE);

		VkPipelineRasterizationStateCreateInfo rasterizationState =
			VkFactory::PipelineRasterizationStateCreateInfo(
				VK_POLYGON_MODE_FILL,
				VK_CULL_MODE_BACK_BIT,
				VK_FRONT_FACE_CLOCKWISE,
				0);

		VkPipelineColorBlendAttachmentState blendAttachmentState =
			VkFactory::PipelineColorBlendAttachmentState(
				0xf,
				VK_FALSE);

		VkPipelineColorBlendStateCreateInfo colorBlendState =
			VkFactory::PipelineColorBlendStateCreateInfo(
				1,
				&blendAttachmentState);

		VkPipelineDepthStencilStateCreateInfo depthStencilState =
			VkFactory::PipelineDepthStencilStateCreateInfo();

		VkPipelineViewportStateCreateInfo viewportState =
			VkFactory::PipelineViewportStateCreateInfo(1, 1);

		VkPipelineMultisampleStateCreateInfo multisampleState =
			VkFactory::PipelineMultisampleStateCreateInfo(
				VK_SAMPLE_COUNT_1_BIT);

		std::vector<VkDynamicState> dynamicStateEnables = {
			VK_DYNAMIC_STATE_VIEWPORT,
			VK_DYNAMIC_STATE_SCISSOR
		};

		VkPipelineDynamicStateCreateInfo dynamicState =
			VkFactory::PipelineDynamicStateCreateInfo(
				dynamicStateEnables.size(),
				dynamicStateEnables.data());


		VkGraphicsPipelineCreateInfo pipelineCreateInfo =
			VkFactory::GraphicsPipelineCreateInfo(
				Shaders::Instance().GetShaderStages(),
				&Vertices::Instance().GetVerticesInput().inputState,
				&inputAssemblyState,
				&viewportState,
				&rasterizationState,
				&multisampleState,
				&depthStencilState,
				&colorBlendState,
				&dynamicState,
				m_pipelineLayout,
				Renderer::Instance().GetRenderpass()->Get()
			);
	
		VK_CHECK_RESULT(vkCreateGraphicsPipelines(LogicalDevice, m_pipelineCache, 1, &pipelineCreateInfo, nullptr, &m_pipelines["phong"]));
	}
	
	Pipelines::~Pipelines()
	{
		for (auto it : m_pipelines)
			vkDestroyPipeline(LogicalDevice, it->second, nullptr);
	}
}