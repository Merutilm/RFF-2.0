//
// Created by Merutilm on 2025-08-15.
//

#include "GPCSlope.hpp"

#include "RCC1.hpp"
#include "SharedDescriptorTemplate.hpp"
#include "../constants/VulkanWindowConstants.hpp"

namespace merutilm::rff2 {
    void GPCSlope::updateQueue(vkh::DescriptorUpdateQueue &queue, const uint32_t frameIndex) {

        //no operation
    }


    void GPCSlope::setSlope(const ShdSlopeAttribute &slope) const {
        using namespace SharedDescriptorTemplate;
        auto &slopeDesc = getDescriptor(SET_SLOPE);
        const auto &slopeUBO = slopeDesc.get<vkh::Uniform>(0, DescSlope::BINDING_UBO_SLOPE);
        auto &slopeUBOHost = slopeUBO->getHostObject();
        slopeUBOHost.set<float>(DescSlope::TARGET_SLOPE_DEPTH, slope.depth);
        slopeUBOHost.set<float>(DescSlope::TARGET_SLOPE_REFLECTION_RATIO, slope.reflectionRatio);
        slopeUBOHost.set<float>(DescSlope::TARGET_SLOPE_OPACITY, slope.opacity);
        slopeUBOHost.set<float>(DescSlope::TARGET_SLOPE_ZENITH, slope.zenith);
        slopeUBOHost.set<float>(DescSlope::TARGET_SLOPE_AZIMUTH, slope.azimuth);
        slopeUBO->update();
    }

    void GPCSlope::pipelineInitialized() {
        using namespace SharedDescriptorTemplate;
        writeDescriptorMF([this](vkh::DescriptorUpdateQueue &queue, const uint32_t frameIndex) {
            getDescriptor(SET_SLOPE).queue(queue, frameIndex, {}, {DescSlope::BINDING_UBO_SLOPE});
        });
    }

    void GPCSlope::windowResized() {
        auto &sic = *engine.getWindowContext(windowAttachmentIndex).sharedImageContext;
        auto &inputDesc = getDescriptor(SET_PREV_RESULT);

        switch (windowAttachmentIndex) {
            case Constants::VulkanWindow::MAIN_WINDOW_ATTACHMENT_INDEX: {
                const auto &input = sic.getImageContextMF(SharedImageContextIndices::MF_MAIN_RENDER_IMAGE_PRIMARY);
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

    void GPCSlope::configurePushConstant(vkh::PipelineLayoutManagerRef pipelineLayoutManager) {
        //noop
    }

    void GPCSlope::configureDescriptors(std::vector<vkh::DescriptorPtr> &descriptors) {
        using namespace SharedDescriptorTemplate;
        auto descManager = vkh::factory::create<vkh::DescriptorManager>();
        descManager->appendInputAttachment(BINDING_PREV_RESULT_INPUT, VK_SHADER_STAGE_FRAGMENT_BIT);

        appendUniqueDescriptor(SET_PREV_RESULT, descriptors, std::move(descManager));
        appendDescriptor<DescIteration>(SET_ITERATION, descriptors);
        appendDescriptor<DescSlope>(SET_SLOPE, descriptors);
    }
}
