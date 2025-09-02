//
// Created by Merutilm on 2025-08-28.
//

#pragma once
#include "../../vulkan_helper/configurator/ComputePipelineConfigurator.hpp"

namespace merutilm::rff2 {
    class CPCBoxBlur final : public vkh::ComputePipelineConfigurator {
        static constexpr uint32_t SET_BLUR_IMAGE = 0;

        static constexpr uint32_t BINDING_BLUR_IMAGE_SRC = 0;
        static constexpr uint32_t BINDING_BLUR_IMAGE_DST = 1;

        static constexpr uint32_t SET_BLUR_RADIUS = 1;
        static constexpr uint32_t BINDING_BLUR_RADIUS_UBO = 0;

        static constexpr uint32_t TARGET_BLUR_UBO_BLUR_SIZE = 0;

        static constexpr uint32_t BLUR_TARGET_COUNT_PER_FRAME = 2;
        static constexpr uint32_t BOX_BLUR_COUNT = 3;

    public:
        explicit CPCBoxBlur(vkh::EngineRef engine);

        ~CPCBoxBlur() override = default;

        CPCBoxBlur(const CPCBoxBlur &) = delete;

        CPCBoxBlur &operator=(const CPCBoxBlur &) = delete;

        CPCBoxBlur(CPCBoxBlur &&) = delete;

        CPCBoxBlur &operator=(CPCBoxBlur &&) = delete;


        void updateQueue(vkh::DescriptorUpdateQueue &queue, uint32_t frameIndex) override;

        void setGaussianBlur(
            const vkh::MultiframeImageContext &srcImage, const vkh::MultiframeImageContext &dstImage, const vkh::
            MultiframeImageContext &tempImage) ;

        void cmdGaussianBlur(
            uint32_t frameIndex, uint32_t blurSizeDescIndex);

        void setImages(uint32_t descIndex, const vkh::MultiframeImageContext &srcImage, const vkh::MultiframeImageContext &dstImage) const;

        void setBlurSize(uint32_t blurSizeDescIndex, float blurSize) const;

        void configure() override {
            ComputePipelineConfigurator::configure();
            initSize();
        }

        void pipelineInitialized() override;

        void windowResized() override;

    protected:
        void initSize() const;

        void configurePushConstant(vkh::PipelineLayoutManagerRef pipelineLayoutManager) override;

        void configureDescriptors(std::vector<vkh::DescriptorPtr> &descriptors) override;
    };
}
