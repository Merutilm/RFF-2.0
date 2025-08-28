//
// Created by Merutilm on 2025-08-28.
//

#pragma once
#include "../../vulkan_helper/configurator/ComputePipelineConfigurator.hpp"

namespace merutilm::rff2 {

    class BoxBlurPipelineConfigurator final : public vkh::ComputePipelineConfigurator{
        static constexpr uint32_t SET_BLUR = 0;
        static constexpr uint32_t BINDING_BLUR_SRC = 0;
        static constexpr uint32_t BINDING_BLUR_DST = 1;
        static constexpr uint32_t BINDING_BLUR_UBO = 2;;
        static constexpr uint32_t TARGET_BLUR_UBO_BLUR_SIZE = 0;
    public:
        explicit BoxBlurPipelineConfigurator(vkh::EngineRef engine);
        ~BoxBlurPipelineConfigurator() override = default;
        BoxBlurPipelineConfigurator(const BoxBlurPipelineConfigurator&) = delete;
        BoxBlurPipelineConfigurator& operator=(const BoxBlurPipelineConfigurator&) = delete;
        BoxBlurPipelineConfigurator(BoxBlurPipelineConfigurator&&) = delete;
        BoxBlurPipelineConfigurator& operator=(BoxBlurPipelineConfigurator&&) = delete;


        void updateQueue(vkh::DescriptorUpdateQueue &queue, uint32_t frameIndex, uint32_t imageIndex) override;

        void setImages(uint32_t imageIndex, const vkh::ImageContext &srcImage, const vkh::ImageContext &dstImage) const;

        void setBlurSize(float blurSize) const;

    protected:
        void configurePushConstant(vkh::PipelineLayoutManagerRef pipelineLayoutManager) override;

        void configureDescriptors(std::vector<vkh::DescriptorPtr> &descriptors) override;
    };

}
