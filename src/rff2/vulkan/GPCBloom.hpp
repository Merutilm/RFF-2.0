//
// Created by Merutilm on 2025-08-30.
//

#pragma once
#include "vulkan_helper/engine/configurator/GeneralPostProcessGraphicsPipelineConfigurator.hpp"
#include "../settings/ShdBloomSettings.h"

namespace merutilm::rff2 {
    struct GPCBloom final : public vkh::GeneralPostProcessGraphicsPipelineConfigurator {
        static constexpr uint32_t SET_BLOOM_CANVAS = 0;
        static constexpr uint32_t BINDING_BLOOM_CANVAS_ORIGINAL = 0;
        static constexpr uint32_t BINDING_BLOOM_CANVAS_BLURRED = 1;

        static constexpr uint32_t SET_BLOOM = 1;

        explicit GPCBloom(vkh::Engine &engine, vkh::WindowContext &wc) : GeneralPostProcessGraphicsPipelineConfigurator(
            engine, wc, "vk_bloom.frag") {
        }

        void updateQueue(vkh::DescriptorUpdateQueue &queue, uint32_t frameIndex) override;

        void setBloom(const ShdBloomSettings &bloom) const;

        void pipelineInitialized() override;

        void renderContextRefreshed() override;

    protected:
        void configurePushConstant(vkh::PipelineLayoutManager & pipelineLayoutManager) override;


        void configureDescriptors(std::vector<vkh::Descriptor *> &descriptors) override;
    };
}
