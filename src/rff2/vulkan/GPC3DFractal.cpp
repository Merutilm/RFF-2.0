//
// Created by Merutilm on 7/27/26.
//

#include "GPC3DFractal.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include "SharedDescriptorTemplate.hpp"
#include "vulkan_helper/engine/configurator/GeneralPostProcessGraphicsPipelineConfigurator.hpp"
#include "vulkan_helper/engine/wrapped/Vertex.hpp"

namespace merutilm::rff2 {
    void GPC3DFractal::updateQueue(vkh::DescriptorUpdateQueue &queue, uint32_t frameIndex) {}

    void GPC3DFractal::cmdRender(const VkCommandBuffer cbh, const uint32_t frameIndex,
                                 vkh::DescIndexPicker &&descIndices) {
        pipeline->cmdBindAll(cbh, specializationIndex, frameIndex, std::move(descIndices));
        cmdPushAll(cbh);
        cmdDraw(cbh, frameIndex, TARGET_IBO);
    }

    void GPC3DFractal::pipelineInitialized() {

        using namespace SharedDescriptorTemplate;
        auto &cameraDesc = getDescriptor(SET_CAMERA);
        auto &fractal3dDesc = getDescriptor(SET_FRACTAL3D);

        writeDescriptorMF([&cameraDesc, &fractal3dDesc](vkh::DescriptorUpdateQueue &queue, const uint32_t frameIndex) {
            cameraDesc.queue(queue, frameIndex, {}, {DescCamera3D::BINDING_UBO_CAMERA});
            fractal3dDesc.queue(queue, frameIndex, {}, {DescFractal3D::BINDING_UBO_F3D});
        });
    }

    void GPC3DFractal::renderContextRefreshed() {
        // noop
    }

    void GPC3DFractal::resetBuffer(const uint32_t width, const uint32_t height) {

        vkh::VertexBuffer &vbo = getVertexBuffer();
        vkh::IndexBuffer &ibo = getIndexBuffer();
        vkh::HostDataObject &vboHost = vbo.getHostObject();
        vkh::HostDataObject &iboHost = ibo.getHostObject();
        const uint32_t verticesCount = width * height;
        const uint32_t squaresCount = (width - 1) * (height - 1);
        const uint32_t indicesCount = squaresCount * 6;
        std::vector<uint32_t> indices;
        indices.reserve(indicesCount);

        for (uint32_t i = 0; i < width - 1; ++i) {
            for (uint32_t j = 0; j < height - 1; ++j) {
                uint32_t vi = j * width + i;
                indices.push_back(vi);
                indices.push_back(vi + width);
                indices.push_back(vi + 1);
                indices.push_back(vi + 1);
                indices.push_back(vi + width);
                indices.push_back(vi + width + 1);
            }
        }

        vboHost.resizeArray<vkh::Vertex>(0, verticesCount);
        iboHost.resizeArray<uint32_t>(0, indicesCount);
        iboHost.set<uint32_t>(0, indices);
        vbo.reloadBuffer();
        ibo.reloadBuffer();
        updateBufferMF([&vbo, &ibo](const uint32_t frameIndex) {
            vbo.updateMF(frameIndex);
            ibo.updateMF(frameIndex);
        });
        vbo.lock(wc.getCommandPool());
        ibo.lock(wc.getCommandPool());
    }

    void GPC3DFractal::setFractal3D(const ShdFractal3DSettings &fractal3DSettings) const {

        using namespace SharedDescriptorTemplate;
        auto &cameraDesc = getDescriptor(SET_CAMERA);
        auto &cameraUBO = cameraDesc.get<vkh::Uniform>(0, DescCamera3D::BINDING_UBO_CAMERA);
        auto &cameraUBOHost = cameraUBO.getHostObject();

        auto &f3dDesc = getDescriptor(SET_FRACTAL3D);
        auto &f3dUBO = f3dDesc.get<vkh::Uniform>(0, DescFractal3D::BINDING_UBO_F3D);
        auto &f3dUBOHost = f3dUBO.getHostObject();


        const float altitudeRad = glm::radians(fractal3DSettings.altitude);
        const float rotationRad = glm::radians(fractal3DSettings.rotation);

        const float distance = fractal3DSettings.distance;;

        const glm::vec3 cameraPos = {distance * std::cos(altitudeRad) * std::sin(rotationRad), distance * std::cos(altitudeRad) * -std::cos(rotationRad), distance * std::sin(altitudeRad)};
        const glm::mat4 view = glm::lookAt(cameraPos, glm::vec3(0.0f), glm::vec3(0, 0, 1));

        const auto &[w, h] = wc.getSwapchain().getSwapchainExtent();
        const float fov = 90.f;
        glm::mat4 proj = glm::infinitePerspective(glm::radians(fov), static_cast<float>(w) / static_cast<float>(h), 0.01f);
        proj[1][1] *= -1;

        cameraUBOHost.set<glm::mat4>(DescCamera3D::TARGET_CAMERA_MODEL, glm::mat4{1.0f});
        cameraUBOHost.set<glm::mat4>(DescCamera3D::TARGET_CAMERA_VIEW, view);
        cameraUBOHost.set<glm::mat4>(DescCamera3D::TARGET_CAMERA_PROJ, proj);


        f3dUBOHost.set<float>(DescFractal3D::TARGET_F3D_BASE_ITERATION, fractal3DSettings.baseIteration);
        f3dUBOHost.set<float>(DescFractal3D::TARGET_F3D_DEPTH_DIVISOR, fractal3DSettings.depthDivisor);
        f3dUBOHost.set<float>(DescFractal3D::TARGET_F3D_ROTATION, rotationRad);

        updateBufferMF([&cameraUBO](const uint32_t frameIndex) { cameraUBO.updateMF(frameIndex); });
        f3dUBO.update();
    }

    std::vector<VkGraphicsPipelineCreateInfo> GPC3DFractal::generatePipelineInfo(const vkh::PipelineManager &pipelineManager,
                                                                    vkh::RenderPass *rp, uint32_t subpass,
                                                                    vkh::GraphicsPipelineConfiguration &pipelineConfiguration) {
        const auto &modules = pipelineManager.shaderModules;

        pipelineConfiguration.shaderStageCreateInfos.resize(modules.size());
        for (size_t i = 0; i < modules.size(); ++i) {
            pipelineConfiguration.shaderStageCreateInfos[i] = {
                    .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
                    .pNext = nullptr,
                    .flags = 0,
                    .stage = modules[i]->getShaderStage(),
                    .module = modules[i]->getShaderModuleHandle(),
                    .pName = "main",
                    .pSpecializationInfo = nullptr};
        }

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
                .depthTestEnable = VK_TRUE,
                .depthWriteEnable = VK_TRUE,
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

        return {{.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
                .pNext = nullptr,
                .flags = 0,
                .stageCount = static_cast<uint32_t>(pipelineConfiguration.shaderStageCreateInfos.size()),
                .pStages = pipelineConfiguration.shaderStageCreateInfos.data(),
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
                .basePipelineIndex = -1}};
    }

    void GPC3DFractal::configurePushConstant(vkh::PipelineLayoutManager &pipelineLayoutManager) {
        // noop
    }

    void GPC3DFractal::configureDescriptors(std::vector<vkh::Descriptor *> &descriptors) {
        using namespace SharedDescriptorTemplate;
        appendDescriptor<DescIteration>(SET_ITERATION, descriptors);
        appendDescriptor<DescPalette>(SET_PALETTE, descriptors);
        appendDescriptor<DescTime>(SET_TIME, descriptors);
        appendDescriptor<DescCamera3D>(SET_CAMERA, descriptors);
        appendDescriptor<DescFractal3D>(SET_FRACTAL3D, descriptors);
        appendDescriptor<DescStripe>(SET_STRIPE, descriptors);
        appendDescriptor<DescSlope>(SET_SLOPE, descriptors);
    }

    void GPC3DFractal::configureVertexBuffer(vkh::HostDataObjectManager &som) {
        som.reserveArray<vkh::Vertex>(TARGET_VBO, 1);
    }

    void GPC3DFractal::configureIndexBuffer(vkh::HostDataObjectManager &som) {
        som.reserveArray<uint32_t>(TARGET_IBO, 1);
    }
} // namespace merutilm::rff2
