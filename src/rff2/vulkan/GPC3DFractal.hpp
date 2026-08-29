//
// Created by Merutilm on 7/27/26.
//

#pragma once
#include "../settings/ShdFractal3DSettings.hpp"
#include "vulkan_helper/engine/configurator/GeneralGraphicsPipelineConfigurator.hpp"
namespace merutilm::rff2 {

    struct GPC3DFractal : vkh::GeneralGraphicsPipelineConfigurator {

        static constexpr uint32_t SET_ITERATION = 0;
        static constexpr uint32_t SET_PALETTE = 1;
        static constexpr uint32_t SET_TIME = 2;
        static constexpr uint32_t SET_CAMERA = 3;
        static constexpr uint32_t SET_FRACTAL3D = 4;
        static constexpr uint32_t SET_STRIPE = 5;
        static constexpr uint32_t SET_SLOPE = 6;

        static constexpr uint32_t TARGET_VBO = 0;
        static constexpr uint32_t TARGET_IBO = 0;


        explicit GPC3DFractal(vkh::Engine &engine, vkh::WindowContext &wc) : GeneralGraphicsPipelineConfigurator(
            engine, wc, "vk_3d_fractal.vert", "vk_3d_fractal.frag") {
        }
        void updateQueue(vkh::DescriptorUpdateQueue &queue, uint32_t frameIndex) override;
        void cmdRender(VkCommandBuffer cbh, uint32_t frameIndex, vkh::DescIndexPicker &&descIndices) override;
        void pipelineInitialized() override;
        void renderContextRefreshed() override;

        void resetPiplineVI(uint32_t width, uint32_t height);

    protected:
        std::vector<VkGraphicsPipelineCreateInfo> generatePipelineInfo(const vkh::PipelineManager &pipelineManager, vkh::RenderPass *rp, uint32_t subpass,
                             vkh::GraphicsPipelineConfiguration &pipelineConfiguration) override;

        void configurePushConstant(vkh::PipelineLayoutManager &pipelineLayoutManager) override;
        void configureDescriptors(std::vector<vkh::Descriptor *> &descriptors) override;
        void configureVertexBuffer(vkh::HostDataObjectManager &som) override;
        void configureIndexBuffer(vkh::HostDataObjectManager &som) override;
    };
}
