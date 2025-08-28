//
// Created by Merutilm on 2025-08-15.
//

#include "ColorPipelineConfigurator.hpp"

#include "RFFFirstRenderContextConfigurator.hpp"
#include "SharedDescriptorTemplate.hpp"
#include "../attr/ShdColorAttribute.h"

namespace merutilm::rff2 {
    void ColorPipelineConfigurator::updateQueue(vkh::DescriptorUpdateQueue &queue, const uint32_t frameIndex,
                                                const uint32_t imageIndex) {
        auto &input = getRenderContextConfigurator<RFFFirstRenderContextConfigurator>().getImageContext(
            RFFFirstRenderContextConfigurator::TEMP_IMAGE_CONTEXT);
        const auto &inputDesc = getDescriptor(SET_PREV_RESULT);
        auto &inputManager = inputDesc.getDescriptorManager();
        inputManager.get<vkh::InputAttachment>(BINDING_PREV_RESULT_INPUT).ctx = input;
        inputDesc.queue(queue, frameIndex, imageIndex);
    }

    void ColorPipelineConfigurator::setColor(const ShdColorAttribute &color) const {
        using namespace SharedDescriptorTemplate;
        const auto &colorDesc = getDescriptor(SET_COLOR);
        auto &colorManager = colorDesc.getDescriptorManager();
        const auto &colorUBO = *colorManager.get<vkh::Uniform>(DescColor::BINDING_UBO_COLOR);
        auto &colorUBOHost = colorUBO.getHostObject();
        colorUBOHost.set<float>(DescColor::TARGET_COLOR_GAMMA, color.gamma);
        colorUBOHost.set<float>(DescColor::TARGET_COLOR_EXPOSURE, color.exposure);
        colorUBOHost.set<float>(DescColor::TARGET_COLOR_HUE, color.hue);
        colorUBOHost.set<float>(DescColor::TARGET_COLOR_SATURATION, color.saturation);
        colorUBOHost.set<float>(DescColor::TARGET_COLOR_BRIGHTNESS, color.brightness);
        colorUBOHost.set<float>(DescColor::TARGET_COLOR_CONTRAST, color.contrast);

        writeDescriptorForEachFrame([&colorUBO, &colorDesc](vkh::DescriptorUpdateQueue &queue, const uint32_t inFlightIndex) {
            colorUBO.update(inFlightIndex);
            colorDesc.queue(queue, inFlightIndex, inFlightIndex);
        });
    }

    void ColorPipelineConfigurator::configurePushConstant(vkh::PipelineLayoutManagerRef pipelineLayoutManager) {
        //noop
    }

    void ColorPipelineConfigurator::configureDescriptors(std::vector<vkh::DescriptorPtr> &descriptors) {
        using namespace SharedDescriptorTemplate;
        auto descManager = vkh::Factory::create<vkh::DescriptorManager>();
        descManager->appendInputAttachment(BINDING_PREV_RESULT_INPUT, VK_SHADER_STAGE_FRAGMENT_BIT);

        appendUniqueDescriptor(SET_PREV_RESULT, descriptors, std::move(descManager));
        appendDescriptor<DescColor>(SET_COLOR, descriptors);
    }
}
