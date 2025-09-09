//
// Created by Merutilm on 2025-08-27.
//

#include "GraphicsPipeline.hpp"

namespace merutilm::vkh {
    GraphicsPipelineImpl::GraphicsPipelineImpl(WindowContextRef wc, PipelineLayoutRef pipelineLayout,
                                               VertexBufferRef vertexBuffer,
                                               IndexBufferRef indexBuffer,
                                               const uint32_t renderContextIndex,
                                               const uint32_t primarySubpassIndex,
                                               PipelineManager &&pipelineManager) : PipelineAbstract(
            wc, pipelineLayout, std::move(pipelineManager)), renderContextIndex(renderContextIndex),
        primarySubpassIndex(primarySubpassIndex),
        vertexBuffer(vertexBuffer),
        indexBuffer(indexBuffer) {
        GraphicsPipelineImpl::init();
    }

    GraphicsPipelineImpl::~GraphicsPipelineImpl() {
        GraphicsPipelineImpl::destroy();
    }

    void GraphicsPipelineImpl::cmdBindAll(const VkCommandBuffer cbh, const uint32_t frameIndex, DescIndexPicker &&descIndices) const {
        const auto sets = enumerateDescriptorSets(frameIndex, std::move(descIndices));
        vkCmdBindPipeline(cbh, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);
        vkCmdBindDescriptorSets(cbh, VK_PIPELINE_BIND_POINT_GRAPHICS,
                                getLayout().getLayoutHandle(), 0,
                                static_cast<uint32_t>(sets.size()), sets.data(), 0,
                                nullptr);
    }


    void GraphicsPipelineImpl::init() {
        auto modules = getShaderModules();

        std::vector<VkPipelineShaderStageCreateInfo> shaderStageCreateInfos(modules.size());
        for (size_t i = 0; i < modules.size(); ++i) {
            shaderStageCreateInfos[i] = {
                .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
                .pNext = nullptr,
                .flags = 0,
                .stage = modules[i]->getShaderStage(),
                .module = modules[i]->getShaderModuleHandle(),
                .pName = "main",
                .pSpecializationInfo = nullptr
            };
        }

        auto &vertInputAttributeDescription = vertexBuffer.getVertexInputAttributeDescriptions();
        auto &vertBindingDescription = vertexBuffer.getVertexInputBindingDescriptions();


        VkPipelineVertexInputStateCreateInfo vertexInputStateCreateInfo = {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .vertexBindingDescriptionCount = static_cast<uint32_t>(vertBindingDescription.size()),
            .pVertexBindingDescriptions = vertBindingDescription.data(),
            .vertexAttributeDescriptionCount = static_cast<uint32_t>(vertInputAttributeDescription.size()),
            .pVertexAttributeDescriptions = vertInputAttributeDescription.data(),
        };

        VkPipelineInputAssemblyStateCreateInfo inputAssemblyStateCreateInfo = {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
            .primitiveRestartEnable = VK_FALSE
        };

        VkPipelineViewportStateCreateInfo viewportStateCreateInfo = {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .viewportCount = 1,
            .pViewports = nullptr,
            .scissorCount = 1,
            .pScissors = nullptr
        };

        VkPipelineRasterizationStateCreateInfo rasterizationStateCreateInfo = {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .depthClampEnable = VK_TRUE,
            .rasterizerDiscardEnable = VK_FALSE,
            .polygonMode = VK_POLYGON_MODE_FILL,
            .cullMode = VK_CULL_MODE_BACK_BIT,
            .frontFace = VK_FRONT_FACE_CLOCKWISE,
            .depthBiasEnable = VK_FALSE,
            .depthBiasConstantFactor = 0,
            .depthBiasClamp = 0,
            .depthBiasSlopeFactor = 0,
            .lineWidth = 1
        };

        VkPipelineMultisampleStateCreateInfo multisampleStateCreateInfo = {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .rasterizationSamples = VK_SAMPLE_COUNT_1_BIT,
            .sampleShadingEnable = VK_FALSE,
            .minSampleShading = 1,
            .pSampleMask = nullptr,
            .alphaToCoverageEnable = VK_FALSE,
            .alphaToOneEnable = VK_FALSE
        };

        VkPipelineDepthStencilStateCreateInfo depthStencilStateCreateInfo = {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .depthTestEnable = VK_FALSE,
            .depthWriteEnable = VK_FALSE,
            .depthCompareOp = VK_COMPARE_OP_LESS,
            .depthBoundsTestEnable = VK_TRUE,
            .stencilTestEnable = VK_FALSE,
            .front = {},
            .back = {},
            .minDepthBounds = 0,
            .maxDepthBounds = 1
        };

        VkPipelineColorBlendAttachmentState colorBlendAttachmentState = {
            .blendEnable = VK_TRUE,
            .srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA,
            .dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,
            .colorBlendOp = VK_BLEND_OP_ADD,
            .srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE,
            .dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO,
            .alphaBlendOp = VK_BLEND_OP_ADD,
            .colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT |
                              VK_COLOR_COMPONENT_A_BIT
        };

        VkPipelineColorBlendStateCreateInfo colorBlendStateCreateInfo = {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .logicOpEnable = VK_FALSE,
            .logicOp = VK_LOGIC_OP_NO_OP,
            .attachmentCount = 1,
            .pAttachments = &colorBlendAttachmentState,
            .blendConstants = {0, 0, 0, 1}
        };
        std::array dynamicStates = {
            VK_DYNAMIC_STATE_VIEWPORT,
            VK_DYNAMIC_STATE_SCISSOR
        };
        VkPipelineDynamicStateCreateInfo dynamicState{
            .sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .dynamicStateCount = static_cast<uint32_t>(dynamicStates.size()),
            .pDynamicStates = dynamicStates.data(),
        };

        VkGraphicsPipelineCreateInfo info = {
            .sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .stageCount = static_cast<uint32_t>(shaderStageCreateInfos.size()),
            .pStages = shaderStageCreateInfos.data(),
            .pVertexInputState = &vertexInputStateCreateInfo,
            .pInputAssemblyState = &inputAssemblyStateCreateInfo,
            .pTessellationState = nullptr,
            .pViewportState = &viewportStateCreateInfo,
            .pRasterizationState = &rasterizationStateCreateInfo,
            .pMultisampleState = &multisampleStateCreateInfo,
            .pDepthStencilState = &depthStencilStateCreateInfo,
            .pColorBlendState = &colorBlendStateCreateInfo,
            .pDynamicState = &dynamicState,
            .layout = pipelineLayout.getLayoutHandle(),
            .renderPass = wc.getRenderContext(renderContextIndex).getRenderPass()->getRenderPassHandle(),
            .subpass = primarySubpassIndex,
            .basePipelineHandle = nullptr,
            .basePipelineIndex = -1
        };


        if (allocator::invoke(vkCreateGraphicsPipelines, wc.core.getLogicalDevice().getLogicalDeviceHandle(), nullptr, 1, &info,
                                      nullptr,
                                      &pipeline) != VK_SUCCESS) {
            throw exception_init("Failed to create graphics pipeline!");
        }
    }
}
