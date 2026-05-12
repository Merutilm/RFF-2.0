//
// Created by Merutilm on 2025-09-09.
//

#pragma once
#include "opencv2/core/mat.hpp"
#include "vulkan_helper/engine/configurator/GeneralPostProcessGraphicsPipelineConfigurator.hpp"

namespace merutilm::rff2 {
    struct GPCStaticImage2Map final : public vkh::GeneralPostProcessGraphicsPipelineConfigurator {
        static constexpr uint32_t SET_IMAGES = 0;
        static constexpr uint32_t BINDING_IMAGES_NORMAL = 0;
        static constexpr uint32_t BINDING_IMAGES_ZOOMED = 1;

        static constexpr uint32_t SET_VIDEO = 1;

        explicit GPCStaticImage2Map(vkh::Engine &engine, const uint32_t windowContextIndex,
                                    const uint32_t renderContextIndex, const uint32_t subpassIndex,
                                    vkh::VertexBuffer &vertexBufferStaticRef, vkh::IndexBuffer &indexBufferStaticRef) :
            GeneralPostProcessGraphicsPipelineConfigurator(engine, windowContextIndex, renderContextIndex, subpassIndex,
                                                           "vk_static_2_image.frag", vertexBufferStaticRef, indexBufferStaticRef) {}

        ~GPCStaticImage2Map() override = default;

        GPCStaticImage2Map(const GPCStaticImage2Map &) = delete;

        GPCStaticImage2Map &operator=(const GPCStaticImage2Map &) = delete;

        GPCStaticImage2Map(GPCStaticImage2Map &&) = delete;

        GPCStaticImage2Map &operator=(GPCStaticImage2Map &&) = delete;

        void updateQueue(vkh::DescriptorUpdateQueue &queue, uint32_t frameIndex) override;

        void pipelineInitialized() override;

        void renderContextRefreshed() override;

        void setImages(const cv::Mat &normal, const cv::Mat &zoomed) const;

    protected:
        void configurePushConstant(vkh::PipelineLayoutManager &pipelineLayoutManager) override;

        void configureDescriptors(std::vector<vkh::Descriptor *> &descriptors) override;
    };
} // namespace merutilm::rff2
