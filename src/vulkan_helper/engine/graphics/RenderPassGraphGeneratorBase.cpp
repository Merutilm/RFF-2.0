//
// Created by Merutilm on 7/9/26.
//

#include <vulkan_helper/engine/graphics/RenderPassGraphGeneratorBase.hpp>
#include <vulkan_helper/engine/pipeline/GraphicsPipelineNode.hpp>
#include <vulkan_helper/engine/configurator/PipelineConfigurator.hpp>

namespace merutilm::vkh {


    RenderPassGraphGeneratorBase::RenderPassGraphGeneratorBase() = default;

    RenderPassGraphGeneratorBase::~RenderPassGraphGeneratorBase() = default;

    void RenderPassGraphGeneratorBase::regenerate() {
        rpm.attachments.clear();
        configureAttachments();
    }

    void RenderPassGraphGeneratorBase::configureAll() {
        configureAttachments();
        configurePipelines();
        generate();
    }
    std::vector<std::unique_ptr<PipelineConfigurator>> RenderPassGraphGeneratorBase::takePipelineConfigurators() {
        return std::exchange(pipelineConfiguratorsCache, {});
    }

} // namespace merutilm::vkh
