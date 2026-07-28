//
// Created by Merutilm on 7/28/26.
//

#pragma once
#include "vulkan_helper/engine/configurator/GeneralPostProcessGraphicsPipelineConfigurator.hpp"

namespace merutilm::rff2 {
    class GPCSmoothZoom : public vkh::GeneralPostProcessGraphicsPipelineConfigurator {
    public:
        GPCSmoothZoom(vkh::Engine &engine, vkh::WindowContext &wc) :
            GeneralPostProcessGraphicsPipelineConfigurator(engine, wc, "vk_smooth_zoom.frag") {
        }

        void updateQueue(vkh::DescriptorUpdateQueue &queue, uint32_t frameIndex) override;
        void pipelineInitialized() override;
        void renderContextRefreshed() override;

    protected:
        void configurePushConstant(vkh::PipelineLayoutManager &pipelineLayoutManager) override;
        void configureDescriptors(std::vector<vkh::Descriptor *> &descriptors) override;
    };
}