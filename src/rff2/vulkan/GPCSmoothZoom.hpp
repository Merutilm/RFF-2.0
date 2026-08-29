//
// Created by Merutilm on 7/28/26.
//

#pragma once
#include "vulkan_helper/engine/configurator/GeneralPostProcessGraphicsPipelineConfigurator.hpp"

namespace merutilm::rff2 {
    class GPCSmoothZoom : public vkh::GeneralPostProcessGraphicsPipelineConfigurator {

        static constexpr uint32_t SET_SAMPLE = 0;
        static constexpr uint32_t BINDING_SAMPLE_SAMPLER = 0;
        static constexpr uint32_t BINDING_SAMPLE_RESOLUTION_UBO = 1;
        static constexpr uint32_t TARGET_SAMPLE_EXTENT = 0;


        static constexpr uint32_t SET_SMOOTH_ZOOM = 1;

    public:
        GPCSmoothZoom(vkh::Engine &engine, vkh::WindowContext &wc) :
            GeneralPostProcessGraphicsPipelineConfigurator(engine, wc, "vk_smooth_zoom.frag") {
        }

        void updateQueue(vkh::DescriptorUpdateQueue &queue, uint32_t frameIndex) override;
        void pipelineInitialized() override;
        void renderContextRefreshed() override;

        void setRescaledResolution(const glm::vec2 &newResolution) const;

    protected:
        void configurePushConstant(vkh::PipelineLayoutManager &pipelineLayoutManager) override;
        void configureDescriptors(std::vector<vkh::Descriptor *> &descriptors) override;
    };
}