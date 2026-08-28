//
// Created by Merutilm on 2025-08-05.
//

#include <vulkan_helper/engine/configurator/GeneralPostProcessGraphicsPipelineConfigurator.hpp>

#include <vulkan_helper/engine/pipeline/GraphicsPipeline.hpp>
#include <vulkan_helper/engine/repo/GlobalPipelineLayoutRepo.hpp>
#include <vulkan_helper/engine/wrapped/Vertex.hpp>

#include "vulkan_helper/engine/wrapped/GraphicsPipelineConfiguration.hpp"

namespace merutilm::vkh {
    void GeneralPostProcessGraphicsPipelineConfigurator::cmdRender(const VkCommandBuffer cbh, const uint32_t frameIndex,
                                                                   DescIndexPicker &&descIndices) {
        pipeline->cmdBindAll(cbh, specializationIndex, frameIndex, std::move(descIndices));
        cmdPushAll(cbh);
        cmdDraw(cbh, frameIndex, 0);
    }


    void GeneralPostProcessGraphicsPipelineConfigurator::configureVertexBuffer(HostDataObjectManager &som) {}

    void GeneralPostProcessGraphicsPipelineConfigurator::configureIndexBuffer(HostDataObjectManager &som) {}

    std::vector<VkGraphicsPipelineCreateInfo> GeneralPostProcessGraphicsPipelineConfigurator::generatePipelineInfo(
            const PipelineManager &pipelineManager, RenderPass *rp, const uint32_t subpass,
            GraphicsPipelineConfiguration &pipelineConfiguration) {


        auto &vertInputAttributeDescription = getVertexBuffer().getVertexInputAttributeDescriptions();
        auto &vertBindingDescription = getVertexBuffer().getVertexInputBindingDescriptions();


        pipelineConfiguration.vertexInputStateCreateInfo = VkPipelineVertexInputStateCreateInfo{
                .sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
                .pNext = nullptr,
                .flags = 0,
                .vertexBindingDescriptionCount = static_cast<uint32_t>(vertBindingDescription.size()),
                .pVertexBindingDescriptions = vertBindingDescription.data(),
                .vertexAttributeDescriptionCount = static_cast<uint32_t>(vertInputAttributeDescription.size()),
                .pVertexAttributeDescriptions = vertInputAttributeDescription.data(),
        };

        pipelineConfiguration.inputAssemblyStateCreateInfo = VkPipelineInputAssemblyStateCreateInfo{
                .sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
                .pNext = nullptr,
                .flags = 0,
                .topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
                .primitiveRestartEnable = VK_FALSE};

        pipelineConfiguration.viewportStateCreateInfo =
                VkPipelineViewportStateCreateInfo{.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
                                                  .pNext = nullptr,
                                                  .flags = 0,
                                                  .viewportCount = 1,
                                                  .pViewports = nullptr,
                                                  .scissorCount = 1,
                                                  .pScissors = nullptr};

        pipelineConfiguration.rasterizationStateCreateInfo = VkPipelineRasterizationStateCreateInfo{
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
                .lineWidth = 1};

        pipelineConfiguration.multisampleStateCreateInfo =
                VkPipelineMultisampleStateCreateInfo{.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
                                                     .pNext = nullptr,
                                                     .flags = 0,
                                                     .rasterizationSamples = VK_SAMPLE_COUNT_1_BIT,
                                                     .sampleShadingEnable = VK_FALSE,
                                                     .minSampleShading = 1,
                                                     .pSampleMask = nullptr,
                                                     .alphaToCoverageEnable = VK_FALSE,
                                                     .alphaToOneEnable = VK_FALSE};

        pipelineConfiguration.depthStencilStateCreateInfo = VkPipelineDepthStencilStateCreateInfo{
                .sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,
                .pNext = nullptr,
                .flags = 0,
                .depthTestEnable = VK_FALSE,
                .depthWriteEnable = VK_FALSE,
                .depthCompareOp = VK_COMPARE_OP_LESS,
                .depthBoundsTestEnable = VK_FALSE,
                .stencilTestEnable = VK_FALSE,
                .front = {},
                .back = {},
                .minDepthBounds = 0,
                .maxDepthBounds = 1};

        pipelineConfiguration.colorBlendAttachmentState = VkPipelineColorBlendAttachmentState{
                .blendEnable = VK_TRUE,
                .srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA,
                .dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,
                .colorBlendOp = VK_BLEND_OP_ADD,
                .srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE,
                .dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO,
                .alphaBlendOp = VK_BLEND_OP_ADD,
                .colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT |
                                  VK_COLOR_COMPONENT_A_BIT};

        pipelineConfiguration.colorBlendStateCreateInfo =
                VkPipelineColorBlendStateCreateInfo{.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
                                                    .pNext = nullptr,
                                                    .flags = 0,
                                                    .logicOpEnable = VK_FALSE,
                                                    .logicOp = VK_LOGIC_OP_NO_OP,
                                                    .attachmentCount = 1,
                                                    .pAttachments = &pipelineConfiguration.colorBlendAttachmentState,
                                                    .blendConstants = {0, 0, 0, 1}};


        pipelineConfiguration.dynamicStates = {VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR};
        pipelineConfiguration.dynamicStateCreateInfo = VkPipelineDynamicStateCreateInfo{
                .sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
                .pNext = nullptr,
                .flags = 0,
                .dynamicStateCount = static_cast<uint32_t>(pipelineConfiguration.dynamicStates.size()),
                .pDynamicStates = pipelineConfiguration.dynamicStates.data(),
        };

        const auto &modules = pipelineManager.shaderModules;

        pipelineConfiguration.specializationInfos = pipelineManager.specialization.buildSpecializationInfo();
        std::vector<VkGraphicsPipelineCreateInfo> createInfos;
        createInfos.reserve(pipelineConfiguration.specializationInfos.size());
        pipelineConfiguration.shaderStageCreateInfos.reserve(pipelineConfiguration.specializationInfos.size());

        for (auto &specializationInfo : pipelineConfiguration.specializationInfos) {
            pipelineConfiguration.shaderStageCreateInfos.emplace_back();
            auto &shaderStageCreateInfos = pipelineConfiguration.shaderStageCreateInfos.back();
            shaderStageCreateInfos.reserve(modules.size());
            for (const auto module : modules) {
                shaderStageCreateInfos.emplace_back(VkPipelineShaderStageCreateInfo{.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
                                             .pNext = nullptr,
                                             .flags = 0,
                                             .stage = module->getShaderStage(),
                                             .module = module->getShaderModuleHandle(),
                                             .pName = "main",
                                             .pSpecializationInfo = pipelineManager.specialization.isEmpty() ? nullptr : &specializationInfo});
            }

            VkGraphicsPipelineCreateInfo info = {.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
                .pNext = nullptr,
                .flags = 0,
                .stageCount = static_cast<uint32_t>(shaderStageCreateInfos.size()),
                .pStages = shaderStageCreateInfos.data(),
                .pVertexInputState = &pipelineConfiguration.vertexInputStateCreateInfo,
                .pInputAssemblyState = &pipelineConfiguration.inputAssemblyStateCreateInfo,
                .pTessellationState = nullptr,
                .pViewportState = &pipelineConfiguration.viewportStateCreateInfo,
                .pRasterizationState = &pipelineConfiguration.rasterizationStateCreateInfo,
                .pMultisampleState = &pipelineConfiguration.multisampleStateCreateInfo,
                .pDepthStencilState = &pipelineConfiguration.depthStencilStateCreateInfo,
                .pColorBlendState = &pipelineConfiguration.colorBlendStateCreateInfo,
                .pDynamicState = &pipelineConfiguration.dynamicStateCreateInfo,
                .layout = pipelineManager.layout->getLayoutHandle(),
                .renderPass = rp->getRenderPassHandle(),
                .subpass = subpass,
                .basePipelineHandle = nullptr,
                .basePipelineIndex = -1};

            createInfos.emplace_back(std::move(info));
        }


        return createInfos;
    }

    void GeneralPostProcessGraphicsPipelineConfigurator::configure(RenderPass *rp, const uint32_t subpass) {


        PipelineLayoutManager pipelineLayoutManager{};

        std::vector<Descriptor *> descriptors = {};
        configureDescriptors(descriptors);

        for (const auto descriptor: descriptors) {
            pipelineLayoutManager.appendDescriptorSetLayout(&descriptor->getLayout());
        }

        configurePushConstant(pipelineLayoutManager);
        PipelineLayout &pipelineLayout = engine.getGlobalRepositories().getRepository<GlobalPipelineLayoutRepo>()->pick(
                std::move(pipelineLayoutManager));


        PipelineManager pipelineManager(pipelineLayout, createSpecializationInfo());

        pipelineManager.attachDescriptor(std::move(descriptors));
        pipelineManager.attachShader(&vertexShader);
        pipelineManager.attachShader(&fragmentShader);

        createPipeline(std::move(pipelineManager), rp, subpass);
    }


} // namespace merutilm::vkh
