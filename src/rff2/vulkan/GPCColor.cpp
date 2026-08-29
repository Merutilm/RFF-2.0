//
// Created by Merutilm on 2025-08-15.
//

#include "GPCColor.hpp"


#include "SharedImageContextIndices.hpp"
#include "desc/SharedDescriptorTemplate.hpp"

namespace merutilm::rff2 {
    void GPCColor::updateQueue(vkh::DescriptorUpdateQueue &queue, const uint32_t frameIndex) {
        //no operation
    }

    void GPCColor::pipelineInitialized() {
        //noop
    }

    void GPCColor::renderContextRefreshed() {
        auto &sic = wc.getSharedImageContext();
        auto &inputDesc = getDescriptor(SET_PREV_RESULT);
        const auto &input =  sic.getImageContextMF(SharedImageContextIndices::MF_MAIN_RENDER_IMAGE_SECONDARY);
        inputDesc.get<vkh::InputAttachment>(0, BINDING_PREV_RESULT_INPUT).ctx = input;

        writeDescriptorMF([&inputDesc](vkh::DescriptorUpdateQueue &queue, const uint32_t frameIndex) {
            inputDesc.queue(queue, frameIndex, {}, {BINDING_PREV_RESULT_INPUT});
        });
    }

    void GPCColor::configurePushConstant(vkh::PipelineLayoutManager &pipelineLayoutManager) {
        //noop
    }

    void GPCColor::configureDescriptors(std::vector<vkh::Descriptor *> &descriptors) {
        using namespace SharedDescriptorTemplate;
        auto descManager = vkh::DescriptorManager();
        descManager.appendInputAttachment(BINDING_PREV_RESULT_INPUT, VK_SHADER_STAGE_FRAGMENT_BIT);

        appendUniqueDescriptor(SET_PREV_RESULT, descriptors, std::move(descManager));
        appendDescriptor<DescColor>(SET_COLOR, descriptors);
    }
}
