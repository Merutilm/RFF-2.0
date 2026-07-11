//
// Created by Merutilm on 2025-08-15.
//

#pragma once
#include "vulkan_helper/engine/configurator/GeneralPostProcessGraphicsPipelineConfigurator.hpp"

namespace merutilm::rff2 {
    struct GPCBloomThreshold final : public vkh::GeneralPostProcessGraphicsPipelineConfigurator {
        static constexpr uint32_t SET_BLOOM_THRESHOLD = 0;
        static constexpr uint32_t BINDING_PREV_RESULT_INPUT = 0;

        static constexpr uint32_t SET_BLOOM = 1;

        explicit GPCBloomThreshold(vkh::Engine &engine, vkh::WindowContext &wc) : GeneralPostProcessGraphicsPipelineConfigurator(
            engine, wc, "vk_bloom_threshold.frag") {
        }

        ~GPCBloomThreshold() override = default;

        GPCBloomThreshold(const GPCBloomThreshold &) = delete;

        GPCBloomThreshold &operator=(const GPCBloomThreshold &) = delete;

        GPCBloomThreshold(GPCBloomThreshold &&) = delete;

        GPCBloomThreshold &operator=(GPCBloomThreshold &&) = delete;

        void updateQueue(vkh::DescriptorUpdateQueue &queue, uint32_t frameIndex) override;

        void pipelineInitialized() override;

        void renderContextRefreshed() override;

    protected:
        void configurePushConstant(vkh::PipelineLayoutManager &pipelineLayoutManager) override;

        void configureDescriptors(std::vector<vkh::Descriptor *> &descriptors) override;
    };
}
