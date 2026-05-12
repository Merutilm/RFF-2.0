//
// Created by Merutilm on 2025-08-15.
//

#pragma once
#include "../settings/ShdSlopeSettings.h"
#include "vulkan_helper/engine/configurator/GeneralPostProcessGraphicsPipelineConfigurator.hpp"

namespace merutilm::rff2 {
    struct GPCSlope final : public vkh::GeneralPostProcessGraphicsPipelineConfigurator {
        static constexpr uint32_t SET_PREV_RESULT = 0;
        static constexpr uint32_t BINDING_PREV_RESULT_SAMPLER = 0;

        static constexpr uint32_t SET_ITERATION = 1;
        static constexpr uint32_t SET_SLOPE = 2;

        explicit GPCSlope(vkh::Engine &engine, const uint32_t windowContextIndex, const uint32_t renderContextIndex,
                          const uint32_t primarySubpassIndex, vkh::VertexBuffer &vertexBufferStaticRef,
                          vkh::IndexBuffer &indexBufferStaticRef) :
            GeneralPostProcessGraphicsPipelineConfigurator(engine, windowContextIndex, renderContextIndex,
                                                           primarySubpassIndex, "vk_slope.frag", vertexBufferStaticRef,
                                                           indexBufferStaticRef) {}

        void updateQueue(vkh::DescriptorUpdateQueue &queue, uint32_t frameIndex) override;

        void setSlope(const ShdSlopeSettings &slope) const;

        void pipelineInitialized() override;

        void renderContextRefreshed() override;

    protected:
        void configurePushConstant(vkh::PipelineLayoutManager &pipelineLayoutManager) override;

        void configureDescriptors(std::vector<vkh::Descriptor *> &descriptors) override;
    };
} // namespace merutilm::rff2
