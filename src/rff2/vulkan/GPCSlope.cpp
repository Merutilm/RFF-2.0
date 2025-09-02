//
// Created by Merutilm on 2025-08-15.
//

#include "GPCSlope.hpp"

#include "RCCFirst.hpp"
#include "SharedDescriptorTemplate.hpp"

namespace merutilm::rff2 {
    void GPCSlope::updateQueue(vkh::DescriptorUpdateQueue &queue, const uint32_t frameIndex) {

        //no operation
    }

    void GPCSlope::setResolution(const glm::uvec2 &swapchainExtent, const float clarityMultiplier) const {
        using namespace SharedDescriptorTemplate;
        auto &resDesc = getDescriptor(SET_RESOLUTION);
        auto &resUBO = resDesc.get<vkh::Uniform>(0, DescResolution::BINDING_UBO_RESOLUTION);
        auto &resUBOHost = resUBO->getHostObject();
        resUBOHost.set<glm::uvec2>(DescResolution::TARGET_RESOLUTION_SWAPCHAIN_EXTENT, swapchainExtent);
        resUBOHost.set<float>(DescResolution::TARGET_RESOLUTION_CLARITY_MULTIPLIER, clarityMultiplier);

        updateBufferForEachFrame([&resUBO](const uint32_t frameIndex) {
            resUBO->update(frameIndex);
        });
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

        updateBufferForEachFrame([&slopeUBO](const uint32_t frameIndex) {
            slopeUBO->update(frameIndex);
        });
    }

    void GPCSlope::pipelineInitialized() {
        using namespace SharedDescriptorTemplate;
        writeDescriptorForEachFrame([this](vkh::DescriptorUpdateQueue &queue, const uint32_t frameIndex) {
            getDescriptor(SET_RESOLUTION).queue(queue, frameIndex, {}, {DescResolution::BINDING_UBO_RESOLUTION});
            getDescriptor(SET_SLOPE).queue(queue, frameIndex, {}, {DescSlope::BINDING_UBO_SLOPE});
        });
    }

    void GPCSlope::windowResized() {
        const auto &input = engine.getRenderContextConfigurator<RCCFirst>().getImageContext(
           RCCFirst::RESULT_IMAGE_CONTEXT);
        auto &inputDesc = getDescriptor(SET_PREV_RESULT);
        inputDesc.get<vkh::InputAttachment>(0, BINDING_PREV_RESULT_INPUT).ctx = input;

        writeDescriptorForEachFrame([&inputDesc](vkh::DescriptorUpdateQueue &queue, const uint32_t frameIndex) {
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
        appendDescriptor<DescResolution>(SET_RESOLUTION, descriptors);
    }
}
