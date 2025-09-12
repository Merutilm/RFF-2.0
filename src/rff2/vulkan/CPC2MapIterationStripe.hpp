//
// Created by Merutilm on 2025-09-06.
//

#pragma once
#include "../../vulkan_helper/configurator/ComputePipelineConfigurator.hpp"
#include "../attr/ShdPaletteAttribute.h"
#include "../attr/ShdStripeAttribute.h"

namespace merutilm::rff2 {
    struct CPC2MapIterationStripe final : public vkh::ComputePipelineConfigurator {

        static constexpr uint32_t SET_I2MAP = 0;
        static constexpr uint32_t BINDING_I2MAP_SSBO_NORMAL = 0;
        static constexpr uint32_t TARGET_I2MAP_SSBO_NORMAL_ITERATION = 0;
        static constexpr uint32_t BINDING_I2MAP_SSBO_ZOOMED = 1;
        static constexpr uint32_t TARGET_I2MAP_SSBO_ZOOMED_ITERATION = 0;
        static constexpr uint32_t SET_VIDEO = 1;
        static constexpr uint32_t SET_PALETTE = 2;
        static constexpr uint32_t SET_TIME = 3;
        static constexpr uint32_t SET_OUTPUT_IMAGE = 4;
        static constexpr uint32_t BINDING_OUTPUT_MERGED_IMAGE = 0;
        static constexpr uint32_t SET_OUTPUT_ITERATION = 5;
        static constexpr uint32_t SET_STRIPE = 6;

        explicit CPC2MapIterationStripe(vkh::EngineRef engine, const uint32_t windowContextIndex)
            : ComputePipelineConfigurator(engine, windowContextIndex, "vk_2_map_iter_stripe.comp") {
        }

        ~CPC2MapIterationStripe() override = default;

        CPC2MapIterationStripe(const CPC2MapIterationStripe &) = delete;

        CPC2MapIterationStripe &operator=(const CPC2MapIterationStripe &) = delete;

        CPC2MapIterationStripe(CPC2MapIterationStripe &&) = delete;

        CPC2MapIterationStripe &operator=(CPC2MapIterationStripe &&) = delete;

        void updateQueue(vkh::DescriptorUpdateQueue &queue, uint32_t frameIndex) override;

        [[nodiscard]] const vkh::ImageContext &getOutputColorImage() const {
            return getDescriptor(SET_OUTPUT_IMAGE).get<vkh::StorageImage>(0, BINDING_OUTPUT_MERGED_IMAGE).ctx[0];
        }

        void pipelineInitialized() override;

        void renderContextRefreshed() override;

        void setCurrentFrame(float currentFrame, uint32_t frameIndex) const;

        void setPalette(const ShdPaletteAttribute &palette) const;

        void setStripe(const ShdStripeAttribute &stripe) const;

        void setDefaultZoomIncrement(float defaultZoomIncrement) const;

        void setAllIterations(const std::vector<double> &normal, const std::vector<double> &zoomed) const;

        void set2MapSize(const VkExtent2D &extent);

        void setInfo(double maxIteration) const;

        void setTime(float currentSec, uint32_t frameIndex) const;

    protected:
        void configurePushConstant(vkh::PipelineLayoutManagerRef pipelineLayoutManager) override;

        void configureDescriptors(std::vector<vkh::DescriptorPtr> &descriptors) override;
    };
}
