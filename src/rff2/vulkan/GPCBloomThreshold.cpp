//
// Created by Merutilm on 2025-08-15.
//

#include "GPCBloomThreshold.hpp"

#include "RCC2.hpp"
#include "SharedDescriptorTemplate.hpp"
#include "../constants/VulkanWindowConstants.hpp"

namespace merutilm::rff2 {
    void GPCBloomThreshold::updateQueue(vkh::DescriptorUpdateQueue &queue, const uint32_t frameIndex) {
        //no operation
    }


    void GPCBloomThreshold::pipelineInitialized() {
        //no operation
    }

    void GPCBloomThreshold::windowResized() {
        auto &sic = *engine.getWindowContext(windowAttachmentIndex).sharedImageContext;
        auto &inputDesc = getDescriptor(SET_BLOOM_THRESHOLD);

        switch (windowAttachmentIndex) {
            case Constants::VulkanWindow::MAIN_WINDOW_ATTACHMENT_INDEX: {
                const auto &input = sic.getImageContextMF(SharedImageContextIndices::MF_MAIN_RENDER_IMAGE_SECONDARY);
                inputDesc.get<vkh::InputAttachment>(0, BINDING_PREV_RESULT_INPUT).ctx = input;
                break;
            }
            case Constants::VulkanWindow::VIDEO_WINDOW_ATTACHMENT_INDEX: {
                //TODO : Video window
                break;
            }
            default: {
                //noop
            }
        }
        writeDescriptorMF([&inputDesc](vkh::DescriptorUpdateQueue &queue, const uint32_t frameIndex) {
            inputDesc.queue(queue, frameIndex, {}, {BINDING_PREV_RESULT_INPUT});
        });
    }

    void GPCBloomThreshold::configurePushConstant(
        vkh::PipelineLayoutManagerRef pipelineLayoutManager) {
        // no operation
    }


    void GPCBloomThreshold::configureDescriptors(std::vector<vkh::DescriptorPtr> &descriptors) {
        using namespace SharedDescriptorTemplate;
        auto descManager = vkh::factory::create<vkh::DescriptorManager>();
        descManager->appendInputAttachment(BINDING_PREV_RESULT_INPUT, VK_SHADER_STAGE_FRAGMENT_BIT);

        appendUniqueDescriptor(SET_BLOOM_THRESHOLD, descriptors, std::move(descManager));
        appendDescriptor<DescBloom>(SET_BLOOM, descriptors);
    }
}
