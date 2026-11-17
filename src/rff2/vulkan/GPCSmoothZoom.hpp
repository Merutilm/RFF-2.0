//
// Created by Merutilm on 7/28/26.
//

#pragma once
#include "vulkan-helper/engine/configurator/GeneralPostProcessGraphicsPipelineConfigurator.hpp"

namespace merutilm::rff2 {
    class GPCSmoothZoom : public vkh::GeneralPostProcessGraphicsPipelineConfigurator {

        static constexpr uint32_t SET_SAMPLE = 0;
        static constexpr uint32_t BINDING_SAMPLE_SAMPLER = 0;


        static constexpr uint32_t SET_SMOOTH_ZOOM = 1;
        static constexpr uint32_t SET_ITERATION = 2;
        static constexpr uint32_t SET_ITERATION_SNAPSHOT = 3;

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