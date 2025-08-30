//
// Created by Merutilm on 2025-08-28.
//

#pragma once
#include "../../vulkan_helper/configurator/ComputePipelineConfigurator.hpp"

namespace merutilm::rff2 {
    class CPCBoxBlur final : public vkh::ComputePipelineConfigurator {
        static constexpr uint32_t SET_BLUR = 0;
        static constexpr uint32_t BINDING_BLUR_SRC = 0;
        static constexpr uint32_t BINDING_BLUR_DST = 1;
        static constexpr uint32_t BINDING_BLUR_UBO = 2;
        static constexpr uint32_t TARGET_BLUR_UBO_BLUR_SIZE = 0;

    public:
        explicit CPCBoxBlur(vkh::EngineRef engine);

        ~CPCBoxBlur() override = default;

        CPCBoxBlur(const CPCBoxBlur &) = delete;

        CPCBoxBlur &operator=(const CPCBoxBlur &) = delete;

        CPCBoxBlur(CPCBoxBlur &&) = delete;

        CPCBoxBlur &operator=(CPCBoxBlur &&) = delete;


        void updateQueue(vkh::DescriptorUpdateQueue &queue, uint32_t frameIndex) override;

        void gaussianBlur(float blurSize, const vkh::ImageContext &srcImage, const vkh::ImageContext &dstImage,
                          const vkh::ImageContext &
                          tempImage, uint32_t frameIndex);

        void setImages(uint32_t frameIndex, const vkh::ImageContext &srcImage, const vkh::ImageContext &dstImage) const;

        void setBlurSize(uint32_t frameIndex, float blurSize) const;

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
