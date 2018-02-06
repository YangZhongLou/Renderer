#include "pipeline.h"
#include "device.h"

namespace Concise
{
	Pipeline::Pipeline(Device * device)
	{
		m_device = device;
		
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
}