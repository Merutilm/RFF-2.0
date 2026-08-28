//
// Created by Merutilm on 8/28/26.
//

#pragma once
#include "vulkan_helper/engine/configurator/ComputePipelineConfigurator.hpp"

namespace merutilm::rff2 {

    struct CPCInterpolateIsolated : vkh::ComputePipelineConfigurator{
        static constexpr uint32_t SET_ITERATION = 0;
        static constexpr uint32_t SET_RENDER_META = 1;
        static constexpr uint32_t SET_BATCH_RESULT = 2;


        explicit CPCInterpolateIsolated(vkh::Engine &engine, vkh::WindowContext &wc) :
            ComputePipelineConfigurator(engine, wc, "vk_interpolate_isolated.comp") {

        }

        void updateQueue(vkh::DescriptorUpdateQueue &queue, uint32_t frameIndex) override;
        void pipelineInitialized() override;
        void renderContextRefreshed() override;

    protected:
        void configurePushConstant(vkh::PipelineLayoutManager &pipelineLayoutManager) override;
        void configureDescriptors(std::vector<vkh::Descriptor *> &descriptors) override;
    };

}