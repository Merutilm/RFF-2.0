//
// Created by Merutilm on 2025-08-27.
//

#include "TempPresentPipelineConfigurator.hpp"

#include "SharedDescriptorTemplate.hpp"

namespace merutilm::rff2 {
      void TempPresentPipelineConfigurator::updateQueue(vkh::DescriptorUpdateQueue &queue, const uint32_t frameIndex,
                                                const uint32_t imageIndex) {
        auto &input = getRenderContextConfigurator<RFFFirstRenderContextConfigurator>().getImageContext(
            RFFFirstRenderContextConfigurator::RESULT_IMAGE_CONTEXT);
        const auto &inputDesc = getDescriptor(SET_PREV_RESULT);
        auto &inputManager = inputDesc.getDescriptorManager();
        inputManager.get<vkh::MultiframeImageContext>(BINDING_PREV_RESULT_INPUT) = input;
        inputDesc.queue(queue, frameIndex, imageIndex);
    }

    void TempPresentPipelineConfigurator::configurePushConstant(vkh::PipelineLayoutManagerRef pipelineLayoutManager) {
        //noop
    }

    void TempPresentPipelineConfigurator::configureDescriptors(std::vector<vkh::DescriptorPtr> &descriptors) {
        using namespace SharedDescriptorTemplate;
        auto descManager = vkh::Factory::create<vkh::DescriptorManager>();
        descManager->appendInputAttachment(BINDING_PREV_RESULT_INPUT, VK_SHADER_STAGE_FRAGMENT_BIT);

        appendUniqueDescriptor(SET_PREV_RESULT, descriptors, std::move(descManager));
    }
}
