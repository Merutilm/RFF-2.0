//
// Created by Merutilm on 2025-07-29.
//

#include "../vulkan/IterationPalettePipelineConfigurator.hpp"

#include "SharedDescriptorTemplate.hpp"
#include "../../vulkan_helper/util/ImageContextUtils.hpp"

namespace merutilm::rff2 {
    IterationPalettePipelineConfigurator::IterationPalettePipelineConfigurator(const vkh::Engine &engine,
                                                                 const uint32_t
                                                                 subpassIndex) : GeneralPostProcessPipelineConfigurator(
        engine, subpassIndex, "vk_iteration_palette.frag") {
        IterationPalettePipelineConfigurator::configure();
    }


    void IterationPalettePipelineConfigurator::updateQueue(vkh::DescriptorUpdateQueue &queue, const uint32_t frameIndex,
                                                    const uint32_t imageIndex,
                                                    const uint32_t width, const uint32_t height) {
        getDescriptor(SET_ITERATION).queue(queue, frameIndex);
        getDescriptor(SET_PALETTE).queue(queue, frameIndex);
    }


    void IterationPalettePipelineConfigurator::configurePushConstant(vkh::PipelineLayoutManager &pipelineLayoutManager) {
        //noop
    }

    void IterationPalettePipelineConfigurator::configureDescriptors(std::vector<const vkh::Descriptor *> &descriptors) {

        appendDescriptor<SharedDescriptorTemplate::DescIteration>(SET_ITERATION, descriptors);
        appendDescriptor<SharedDescriptorTemplate::DescPalette>(SET_PALETTE, descriptors);
    }
}
