//
// Created by Merutilm on 2025-08-31.
//

#pragma once
#include "vulkan_helper/engine/configurator/GeneralPostProcessGraphicsPipelineConfigurator.hpp"

namespace merutilm::rff2 {
    struct GPCLinearInterpolation final : public vkh::GeneralPostProcessGraphicsPipelineConfigurator {
        static constexpr uint32_t SET_PREV_RESULT = 0;
        static constexpr uint32_t BINDING_PREV_RESULT_SAMPLER = 0;

        static constexpr uint32_t SET_LINEAR_INTERPOLATION = 1;

        explicit GPCLinearInterpolation(vkh::Engine &engine, vkh::WindowContext &wc) :
            GeneralPostProcessGraphicsPipelineConfigurator(engine, wc,
                                                           "vk_linear_interpolation.frag") {}


        void updateQueue(vkh::DescriptorUpdateQueue &queue, uint32_t frameIndex) override;

        void setLinearInterpolation(bool use) const;

        void pipelineInitialized() override;

        void renderContextRefreshed() override;

    protected:
        void configurePushConstant(vkh::PipelineLayoutManager &pipelineLayoutManager) override;

        void configureDescriptors(std::vector<vkh::Descriptor *> &descriptors) override;
    };
} // namespace merutilm::rff2
