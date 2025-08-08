//
// Created by Merutilm on 2025-07-29.
//

#pragma once
#include "GeneralPipelineConfigurator.hpp"

namespace merutilm::mvk {
    class Template1PipelineConfigurator final : public GeneralPipelineConfigurator{

        static constexpr uint32_t SET_SAMPLE_IMAGE = 0;


    public:
        using GeneralPipelineConfigurator::GeneralPipelineConfigurator;

        ~Template1PipelineConfigurator() override = default;

        Template1PipelineConfigurator(const Template1PipelineConfigurator&) = delete;

        Template1PipelineConfigurator& operator=(const Template1PipelineConfigurator&) = delete;

        Template1PipelineConfigurator(Template1PipelineConfigurator&&) = delete;

        Template1PipelineConfigurator& operator=(Template1PipelineConfigurator&&) = delete;

        void updateQueue(DescriptorUpdateQueue &queue, uint32_t frameIndex, uint32_t imageIndex, uint32_t width, uint32_t height) override;

        void render(VkCommandBuffer cbh, uint32_t frameIndex, uint32_t width, uint32_t height) override;

    protected:
        void configurePushConstant(DescriptorSetLayoutRepo &layoutRepo,
            PipelineLayoutManager &pipelineLayoutManager) override;

        void configureDescriptors(std::vector<const Descriptor *> &descriptors, DescriptorSetLayoutRepo &layoutRepo,
            SharedDescriptorRepo &descRepo) override;
        void configureVertexBuffer(ShaderObjectManager &som) override;

        void configureIndexBuffer(ShaderObjectManager &som) override;
    };
}
