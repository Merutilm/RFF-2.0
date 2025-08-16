//
// Created by Merutilm on 2025-07-20.
//

#pragma once
#include "../../vulkan_helper/configurator/GeneralPostProcessPipelineConfigurator.hpp"
#include "../../vulkan_helper/repo/SharedDescriptorRepo.hpp"

namespace merutilm::rff2 {
    class Template2PipelineConfigurator final : public vkh::GeneralPostProcessPipelineConfigurator {
        static constexpr uint32_t SET_TIME = 0;
        static constexpr uint32_t SET_INPUT = 1;

        static constexpr uint32_t BINDING_INPUT_SAMPLER = 0;
        static constexpr uint32_t BINDING_INPUT_ATTACHMENT_ELEM = 1;

        static constexpr uint32_t PUSH_RESOLUTION = 0;
        static constexpr uint32_t PUSH_RESOLUTION_SIZE = 0;

    public:
        Template2PipelineConfigurator(const vkh::Engine &engine, uint32_t subpassIndex);

        ~Template2PipelineConfigurator() override;

        Template2PipelineConfigurator(const Template2PipelineConfigurator &) = delete;

        Template2PipelineConfigurator &operator=(const Template2PipelineConfigurator &) = delete;

        Template2PipelineConfigurator(Template2PipelineConfigurator &&) = delete;

        Template2PipelineConfigurator &operator=(Template2PipelineConfigurator &&) = delete;

        void updateQueue(vkh::DescriptorUpdateQueue &queue, uint32_t frameIndex, uint32_t imageIndex, uint32_t width,
                         uint32_t height) override;

    protected:
        void configureDescriptors(std::vector<const vkh::Descriptor *> &descriptors) override;

        void configurePushConstant(vkh::PipelineLayoutManager &pipelineLayoutManager) override;

        void init() override;

        void destroy() override;
    };
}
