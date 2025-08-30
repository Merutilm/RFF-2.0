//
// Created by Merutilm on 2025-08-15.
//

#include "GPCBloomThreshold.hpp"

#include "RCCFog.hpp"
#include "SharedDescriptorTemplate.hpp"

namespace merutilm::rff2 {
    void GPCBloomThreshold::updateQueue(vkh::DescriptorUpdateQueue &queue, const uint32_t frameIndex) {
        //no operation
    }


    void GPCBloomThreshold::pipelineInitialized() {
        //no operation
    }

    void GPCBloomThreshold::windowResized() {

        const auto &input = engine.getRenderContextConfigurator<RCCFog>().getImageContext(
                    RCCFog::RESULT_IMAGE_CONTEXT);
        const auto &inputDesc = getDescriptor(SET_BLOOM_THRESHOLD);
        auto &inputManager = inputDesc.getDescriptorManager();
        inputManager.get<vkh::InputAttachment>(BINDING_PREV_RESULT_INPUT).ctx = input;

        writeDescriptorForEachFrame([&inputDesc](vkh::DescriptorUpdateQueue &queue, const uint32_t frameIndex) {
            inputDesc.queue(queue, frameIndex);
        });
    }

    void GPCBloomThreshold::configurePushConstant(
        vkh::PipelineLayoutManagerRef pipelineLayoutManager) {
        // no operation
    }


    void GPCBloomThreshold::configureDescriptors(std::vector<vkh::DescriptorPtr> &descriptors) {
        using namespace SharedDescriptorTemplate;
        auto descManager = vkh::Factory::create<vkh::DescriptorManager>();
        descManager->appendInputAttachment(BINDING_PREV_RESULT_INPUT, VK_SHADER_STAGE_FRAGMENT_BIT);

        appendUniqueDescriptor(SET_BLOOM_THRESHOLD, descriptors, std::move(descManager));
        appendDescriptor<DescBloom>(SET_BLOOM, descriptors);
        appendDescriptor<DescResolution>(SET_RESOLUTION, descriptors);
    }
}
