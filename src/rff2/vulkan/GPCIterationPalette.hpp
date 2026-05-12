//
// Created by Merutilm on 2025-07-29.
//

#pragma once

#include "../settings/ShdPaletteSettings.h"
#include "vulkan_helper/engine/configurator/GeneralPostProcessGraphicsPipelineConfigurator.hpp"

namespace merutilm::rff2 {
    struct GPCIterationPalette final : public vkh::GeneralPostProcessGraphicsPipelineConfigurator {
        static constexpr uint32_t SET_ITERATION = 0;
        static constexpr uint32_t SET_PALETTE = 1;
        static constexpr uint32_t SET_TIME = 2;
        static constexpr uint32_t SET_COLORING = 3;

        uint32_t iterWidth = 0;
        uint32_t iterHeight = 0;

        GPCIterationPalette(vkh::Engine &engine, const uint32_t windowContextIndex, const uint32_t renderContextIndex,
                            const uint32_t primarySubpassIndex, vkh::VertexBuffer &vertexBufferStaticRef,
                            vkh::IndexBuffer &indexBufferStaticRef) :
            GeneralPostProcessGraphicsPipelineConfigurator(engine, windowContextIndex, renderContextIndex,
                                                           primarySubpassIndex, "vk_iteration_palette.frag",
                                                           vertexBufferStaticRef, indexBufferStaticRef) {};

        ~GPCIterationPalette() override = default;

        GPCIterationPalette(const GPCIterationPalette &) = delete;

        GPCIterationPalette &operator=(const GPCIterationPalette &) = delete;

        GPCIterationPalette(GPCIterationPalette &&) = delete;

        GPCIterationPalette &operator=(GPCIterationPalette &&) = delete;

        void updateQueue(vkh::DescriptorUpdateQueue &queue, uint32_t frameIndex) override;

        void cmdRefreshIterations(VkCommandBuffer cbh, const vkh::BufferContext &src) const;

        const vkh::BufferContext &getResultIterationBuffer() const;

        void resetIterationBuffer(uint32_t width, uint32_t height);

        void setMaxIteration(double maxIteration) const;

        void setPalette(const ShdPaletteSettings &palette) const;


        void pipelineInitialized() override;

        void renderContextRefreshed() override;

    protected:
        void configurePushConstant(vkh::PipelineLayoutManager &pipelineLayoutManager) override;

        void configureDescriptors(std::vector<vkh::Descriptor *> &descriptors) override;
    };
} // namespace merutilm::rff2
