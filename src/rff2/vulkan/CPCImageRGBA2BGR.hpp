//
// Created by Merutilm on 2025-09-10.
//

#pragma once
#include "../../vulkan_helper/configurator/ComputePipelineConfigurator.hpp"

namespace merutilm::rff2{
    struct CPCImageRGBA2BGR final : public vkh::ComputePipelineConfigurator {
        static constexpr uint32_t SET_INFO = 0;
        static constexpr uint32_t BINDING_PREV_IMAGE_SAMPLER = 0;
        static constexpr uint32_t BINDING_OUTPUT_SSBO = 1;
        static constexpr uint32_t TARGET_OUTPUT_SSBO_DATA = 0;

        explicit CPCImageRGBA2BGR(vkh::EngineRef engine, const uint32_t windowContextIndex) : ComputePipelineConfigurator(
            engine, windowContextIndex, "vk_image_rgba2bgr.comp") {
        }

        void updateQueue(vkh::DescriptorUpdateQueue &queue, uint32_t frameIndex) override;

        void pipelineInitialized() override;

        void renderContextRefreshed() override;

        [[nodiscard]] const vkh::BufferContext &getBufferContext(uint32_t frameIndex) const;

    protected:
        void configurePushConstant(vkh::PipelineLayoutManagerRef pipelineLayoutManager) override;

        void configureDescriptors(std::vector<vkh::DescriptorPtr> &descriptors) override;
    };

}