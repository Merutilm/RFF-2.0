//
// Created by Merutilm on 2025-07-29.
//

#pragma once
#include "../vulkan_helper/configurator/GeneralPostProcessPipelineConfigurator.hpp"

namespace merutilm::rff2 {
    class Template1PipelineConfigurator final : public mvk::GeneralPostProcessPipelineConfigurator {
        static constexpr uint32_t SET_SAMPLE_IMAGE = 0;

    public:
        Template1PipelineConfigurator(const mvk::Engine &engine,
                                      uint32_t subpassIndex);

        ~Template1PipelineConfigurator() override = default;

        Template1PipelineConfigurator(const Template1PipelineConfigurator &) = delete;

        Template1PipelineConfigurator &operator=(const Template1PipelineConfigurator &) = delete;

        Template1PipelineConfigurator(Template1PipelineConfigurator &&) = delete;

        Template1PipelineConfigurator &operator=(Template1PipelineConfigurator &&) = delete;

        void updateQueue(mvk::DescriptorUpdateQueue &queue, uint32_t frameIndex, uint32_t imageIndex, uint32_t width,
                         uint32_t height) override;

        void render(VkCommandBuffer cbh, uint32_t frameIndex, uint32_t width, uint32_t height) override;

    protected:
        void configurePushConstant(mvk::DescriptorSetLayoutRepo &layoutRepo,
                                   mvk::PipelineLayoutManager &pipelineLayoutManager) override;

        void configureDescriptors(std::vector<const mvk::Descriptor *> &descriptors, mvk::DescriptorSetLayoutRepo &layoutRepo,
                                  mvk::SharedDescriptorRepo &descRepo) override;

    };
}
