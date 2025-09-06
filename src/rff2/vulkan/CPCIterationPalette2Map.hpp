//
// Created by Merutilm on 2025-09-06.
//

#pragma once
#include "../../vulkan_helper/configurator/ComputePipelineConfigurator.hpp"

namespace merutilm::rff2 {
    struct CPCIterationPalette2Map final : public vkh::ComputePipelineConfigurator {

        static constexpr uint32_t SET_I2MAP = 0;
        static constexpr uint32_t BINDING_I2MAP_SSBO_NORMAL = 0;
        static constexpr uint32_t TARGET_I2MAP_SSBO_NORMAL_ITERATION = 0;
        static constexpr uint32_t BINDING_I2MAP_SSBO_ZOOMED = 1;
        static constexpr uint32_t TARGET_I2MAP_SSBO_ZOOMED_ITERATION = 1;
        static constexpr uint32_t BINDING_I2MAP_UBO_ATTR = 2;
        static constexpr uint32_t TARGET_I2MAP_UBO_ATTR_EXTENT = 0;
        static constexpr uint32_t TARGET_I2MAP_UBO_ATTR_MAX_ITERATION = 1;
        static constexpr uint32_t TARGET_I2MAP_UBO_ATTR_TIME = 2;
        static constexpr uint32_t SET_VIDEO = 1;
        static constexpr uint32_t SET_PALETTE = 2;
        static constexpr uint32_t SET_OUTPUT = 3;
        static constexpr uint32_t BINDING_OUTPUT_MERGED_IMAGE = 0;
        static constexpr uint32_t BINDING_OUTPUT_SSBO_RESULT = 1;
        static constexpr uint32_t TARGET_OUTPUT_SSBO_RESULT_ITERATION = 0;

        CPCIterationPalette2Map(vkh::EngineRef engine, const uint32_t windowContextIndex)
            : ComputePipelineConfigurator(engine, windowContextIndex, "vk_iteration_palette_2_map.comp") {
        }

        ~CPCIterationPalette2Map() override = default;

        CPCIterationPalette2Map(const CPCIterationPalette2Map &) = delete;

        CPCIterationPalette2Map &operator=(const CPCIterationPalette2Map &) = delete;

        CPCIterationPalette2Map(CPCIterationPalette2Map &&) = delete;

        CPCIterationPalette2Map &operator=(CPCIterationPalette2Map &&) = delete;

        void updateQueue(vkh::DescriptorUpdateQueue &queue, uint32_t frameIndex) override;

        [[nodiscard]] const vkh::ImageContext &getOutputColorImage() const {
            return getDescriptor(SET_OUTPUT).get<vkh::StorageImage>(0, BINDING_OUTPUT_MERGED_IMAGE).ctx[0];
        }

        void pipelineInitialized() override;

        void windowResized() override;

        void CPCIterationPalette2Map::setInfo(const glm::uvec2 &extent, double maxIteration, float time) const;

    protected:
        void configurePushConstant(vkh::PipelineLayoutManagerRef pipelineLayoutManager) override;

        void configureDescriptors(std::vector<vkh::DescriptorPtr> &descriptors) override;
    };
}
