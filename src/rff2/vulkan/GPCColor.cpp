//
// Created by Merutilm on 2025-08-15.
//

#include "GPCColor.hpp"

#include "RCC1.hpp"
#include "SharedDescriptorTemplate.hpp"
#include "../attr/ShdColorAttribute.h"

namespace merutilm::rff2 {
    void GPCColor::updateQueue(vkh::DescriptorUpdateQueue &queue, const uint32_t frameIndex) {
       //no operation
    }

    void GPCColor::setColor(const ShdColorAttribute &color) const {
        using namespace SharedDescriptorTemplate;
        auto &colorDesc = getDescriptor(SET_COLOR);
        const auto &colorUBO = *colorDesc.get<vkh::Uniform>(0, DescColor::BINDING_UBO_COLOR);
        auto &colorUBOHost = colorUBO.getHostObject();
        colorUBOHost.set<float>(DescColor::TARGET_COLOR_GAMMA, color.gamma);
        colorUBOHost.set<float>(DescColor::TARGET_COLOR_EXPOSURE, color.exposure);
        colorUBOHost.set<float>(DescColor::TARGET_COLOR_HUE, color.hue);
        colorUBOHost.set<float>(DescColor::TARGET_COLOR_SATURATION, color.saturation);
        colorUBOHost.set<float>(DescColor::TARGET_COLOR_BRIGHTNESS, color.brightness);
        colorUBOHost.set<float>(DescColor::TARGET_COLOR_CONTRAST, color.contrast);

        updateBufferForEachFrame([&colorUBO](const uint32_t frameIndex) {
            colorUBO.update(frameIndex);
        });
    }

    void GPCColor::pipelineInitialized() {
        using namespace SharedDescriptorTemplate;
        writeDescriptorForEachFrame([this](vkh::DescriptorUpdateQueue &queue, const uint32_t frameIndex) {
            getDescriptor(SET_COLOR).queue(queue, frameIndex, {}, {DescColor::BINDING_UBO_COLOR});
        });
    }

    void GPCColor::windowResized() {
        const auto &input =  engine.getSharedImageContext().getMultiframeContext(SharedImageContextIndices::MF_RENDER_IMAGE_SECONDARY);
        auto &inputDesc = getDescriptor(SET_PREV_RESULT);
        inputDesc.get<vkh::InputAttachment>(0, BINDING_PREV_RESULT_INPUT).ctx = input;
        writeDescriptorForEachFrame([&inputDesc](vkh::DescriptorUpdateQueue &queue, const uint32_t frameIndex) {
            inputDesc.queue(queue, frameIndex, {}, {BINDING_PREV_RESULT_INPUT});
        });
    }

    void GPCColor::configurePushConstant(vkh::PipelineLayoutManagerRef pipelineLayoutManager) {
        //noop
    }

    void GPCColor::configureDescriptors(std::vector<vkh::DescriptorPtr> &descriptors) {
        using namespace SharedDescriptorTemplate;
        auto descManager = vkh::factory::create<vkh::DescriptorManager>();
        descManager->appendInputAttachment(BINDING_PREV_RESULT_INPUT, VK_SHADER_STAGE_FRAGMENT_BIT);

        appendUniqueDescriptor(SET_PREV_RESULT, descriptors, std::move(descManager));
        appendDescriptor<DescColor>(SET_COLOR, descriptors);
    }
}
