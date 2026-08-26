//
// Created by Merutilm on 8/21/26.
//
#pragma once
#include <vector>
#include "vulkan/vulkan.h"

namespace merutilm::vkh {
    struct GraphicsPipelineConfiguration {
        std::vector<VkPipelineShaderStageCreateInfo> shaderStageCreateInfos;
        VkPipelineVertexInputStateCreateInfo vertexInputStateCreateInfo;
        VkPipelineInputAssemblyStateCreateInfo inputAssemblyStateCreateInfo;
        VkPipelineTessellationStateCreateInfo tessellationStateCreateInfo;
        VkPipelineViewportStateCreateInfo viewportStateCreateInfo;
        VkPipelineRasterizationStateCreateInfo rasterizationStateCreateInfo;
        VkPipelineMultisampleStateCreateInfo multisampleStateCreateInfo;
        VkPipelineDepthStencilStateCreateInfo depthStencilStateCreateInfo;
        VkPipelineColorBlendAttachmentState colorBlendAttachmentState;
        VkPipelineColorBlendStateCreateInfo colorBlendStateCreateInfo;
        std::vector<VkSpecializationInfo> specializationInfo;

        std::vector<VkDynamicState> dynamicStates;
        VkPipelineDynamicStateCreateInfo dynamicStateCreateInfo;
    };
} // namespace merutilm::vkh
