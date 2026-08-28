//
// Created by Merutilm on 2025-07-29.
//

#pragma once

#include "../settings/PerturbationMainIterator.hpp"
#include "../settings/ShdPaletteSettings.h"
#include "vulkan_helper/engine/configurator/GeneralPostProcessGraphicsPipelineConfigurator.hpp"

namespace merutilm::rff2 {
    struct GPCIterationPalette final : public vkh::GeneralPostProcessGraphicsPipelineConfigurator {
        static constexpr uint32_t SET_ITERATION = 0;
        static constexpr uint32_t SET_PALETTE = 1;
        static constexpr uint32_t SET_TIME = 2;
        static constexpr uint32_t SET_BATCH_RESULT = 3;
        static constexpr uint32_t SET_SMOOTH_ZOOM = 4;


        static constexpr uint32_t SPECIALIZATION_PERTURBATION_MAIN_ITERATOR = 0;

        uint32_t iterWidth = 0;
        uint32_t iterHeight = 0;

        GPCIterationPalette(vkh::Engine &engine, vkh::WindowContext &wc) :
            GeneralPostProcessGraphicsPipelineConfigurator(engine, wc, "vk_iteration_palette.frag") {}

        ~GPCIterationPalette() override = default;

        GPCIterationPalette(const GPCIterationPalette &) = delete;

        GPCIterationPalette &operator=(const GPCIterationPalette &) = delete;

        GPCIterationPalette(GPCIterationPalette &&) = delete;

        GPCIterationPalette &operator=(GPCIterationPalette &&) = delete;

        vkh::PipelineSpecialization createSpecializationInfo() override;

        void updateQueue(vkh::DescriptorUpdateQueue &queue, uint32_t frameIndex) override;

        void setPerturbationMainIterator(PerturbationMainIterator mainIterator);

        void cmdRefreshIterations(const vkh::CommandBuffer &commandBuffer, const vkh::BufferContext &src) const;

        [[nodiscard]] const vkh::BufferContext &getResultIterationBuffer() const;

        void resetIterationBuffer(uint32_t width, uint32_t height);

        void applyMaxIteration() const;

        /**
         * call applyMaxIteration to update params for gpu
         */
        void setMaxIteration(double maxIteration) const;

        void setPalette(const ShdPaletteSettings &palette) const;


        void pipelineInitialized() override;

        void renderContextRefreshed() override;

    protected:
        void configurePushConstant(vkh::PipelineLayoutManager &pipelineLayoutManager) override;

        void configureDescriptors(std::vector<vkh::Descriptor *> &descriptors) override;
    };
} // namespace merutilm::rff2
