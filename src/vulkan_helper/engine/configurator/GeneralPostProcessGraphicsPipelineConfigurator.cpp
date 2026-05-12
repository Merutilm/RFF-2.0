//
// Created by Merutilm on 2025-08-05.
//

#include <vulkan_helper/engine/configurator/GeneralPostProcessGraphicsPipelineConfigurator.hpp>

#include <vulkan_helper/base/vkh_base.hpp>
#include <vulkan_helper/engine/pipeline/GraphicsPipeline.hpp>
#include <vulkan_helper/engine/repo/GlobalPipelineLayoutRepo.hpp>
#include <vulkan_helper/engine/wrapped/Vertex.hpp>

namespace merutilm::vkh {
    void GeneralPostProcessGraphicsPipelineConfigurator::cmdRender(const VkCommandBuffer cbh, const uint32_t frameIndex,
                                                                   DescIndexPicker &&descIndices) {
        pipeline->cmdBindAll(cbh, frameIndex, std::move(descIndices));
        cmdPushAll(cbh);
        cmdDraw(cbh, frameIndex, 0);
    }


    void GeneralPostProcessGraphicsPipelineConfigurator::configureVertexBuffer(HostDataObjectManager &som) {
    }

    void GeneralPostProcessGraphicsPipelineConfigurator::configureIndexBuffer(HostDataObjectManager &som) {
    }

    void GeneralPostProcessGraphicsPipelineConfigurator::configure() {


        PipelineLayoutManager pipelineLayoutManager{};

        std::vector<Descriptor *> descriptors = {};
        configureDescriptors(descriptors);

        for (const auto descriptor: descriptors) {
            pipelineLayoutManager.appendDescriptorSetLayout(&descriptor->getLayout());
        }

        configurePushConstant(pipelineLayoutManager);
        PipelineLayout &pipelineLayout = engine.getGlobalRepositories().getRepository<GlobalPipelineLayoutRepo>()->pick(
                std::move(pipelineLayoutManager));


        PipelineManager pipelineManager(pipelineLayout);

        pipelineManager.attachDescriptor(std::move(descriptors));
        pipelineManager.attachShader(&vertexShader);
        pipelineManager.attachShader(&fragmentShader);

        pipeline = std::make_unique<GraphicsPipeline>(wc, pipelineLayout, vertexBufferStaticRef, indexBufferStaticRef,
            renderContextIndex, primarySubpassIndex,
            std::move(pipelineManager));

    }


} // namespace merutilm::vkh
