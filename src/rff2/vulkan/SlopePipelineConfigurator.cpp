//
// Created by Merutilm on 2025-08-15.
//

#include "SlopePipelineConfigurator.hpp"

#include "RFFFirstRenderContextConfigurator.hpp"
#include "SharedDescriptorTemplate.hpp"

namespace merutilm::rff2 {
    void SlopePipelineConfigurator::updateQueue(vkh::DescriptorUpdateQueue &queue, const uint32_t frameIndex,
                                                const uint32_t imageIndex) {

        auto &input = getRenderContextConfigurator<RFFFirstRenderContextConfigurator>().getImageContext(
           RFFFirstRenderContextConfigurator::RESULT_IMAGE_CONTEXT);
        const auto &inputDesc = getDescriptor(SET_PREV_RESULT);
        auto &inputManager = inputDesc.getDescriptorManager();
        inputManager.get<vkh::MultiframeImageContext>(BINDING_PREV_RESULT_INPUT) = input;
        inputDesc.queue(queue, frameIndex, imageIndex);
    }

    void SlopePipelineConfigurator::setResolution(const glm::uvec2 &swapchainExtent, const float clarityMultiplier) const {
        using namespace SharedDescriptorTemplate;
        auto &resDesc = getDescriptor(SET_RESOLUTION);
        auto &resManager = resDesc.getDescriptorManager();
        auto &resUBO = resManager.get<vkh::Uniform>(DescResolution::BINDING_UBO_RESOLUTION);
        auto &resUBOHost = resUBO->getHostObject();
        resUBOHost.set<glm::uvec2>(DescResolution::TARGET_RESOLUTION_SWAPCHAIN_EXTENT, swapchainExtent);
        resUBOHost.set<float>(DescResolution::TARGET_RESOLUTION_CLARITY_MULTIPLIER, clarityMultiplier);

        writeDescriptorForEachFrame([&resUBO, &resDesc](vkh::DescriptorUpdateQueue &queue, const uint32_t inFlightIndex) {
            resUBO->update(inFlightIndex);
            resDesc.queue(queue, inFlightIndex, inFlightIndex);
        });
    }

    void SlopePipelineConfigurator::setSlope(const ShdSlopeAttribute &slope) const {
        using namespace SharedDescriptorTemplate;
        const auto &slopeDesc = getDescriptor(SET_SLOPE);
        auto &slopeManager = slopeDesc.getDescriptorManager();
        const auto &slopeUBO = slopeManager.get<vkh::Uniform>(DescSlope::BINDING_UBO_SLOPE);
        auto &slopeUBOHost = slopeUBO->getHostObject();
        slopeUBOHost.set<float>(DescSlope::TARGET_SLOPE_DEPTH, slope.depth);
        slopeUBOHost.set<float>(DescSlope::TARGET_SLOPE_REFLECTION_RATIO, slope.reflectionRatio);
        slopeUBOHost.set<float>(DescSlope::TARGET_SLOPE_OPACITY, slope.opacity);
        slopeUBOHost.set<float>(DescSlope::TARGET_SLOPE_ZENITH, slope.zenith);
        slopeUBOHost.set<float>(DescSlope::TARGET_SLOPE_AZIMUTH, slope.azimuth);

        writeDescriptorForEachFrame([&slopeUBO, &slopeDesc](vkh::DescriptorUpdateQueue &queue, const uint32_t inFlightIndex) {
            slopeUBO->update(inFlightIndex);
            slopeDesc.queue(queue, inFlightIndex, inFlightIndex);
        });
    }

    void SlopePipelineConfigurator::configurePushConstant(vkh::PipelineLayoutManagerRef pipelineLayoutManager) {
        //noop
    }

    void SlopePipelineConfigurator::configureDescriptors(std::vector<vkh::DescriptorPtr> &descriptors) {
        using namespace SharedDescriptorTemplate;
        auto descManager = vkh::Factory::create<vkh::DescriptorManager>();
        descManager->appendInputAttachment(BINDING_PREV_RESULT_INPUT, VK_SHADER_STAGE_FRAGMENT_BIT);

        appendUniqueDescriptor(SET_PREV_RESULT, descriptors, std::move(descManager));
        appendDescriptor<DescIteration>(SET_ITERATION, descriptors);
        appendDescriptor<DescSlope>(SET_SLOPE, descriptors);
        appendDescriptor<DescResolution>(SET_RESOLUTION, descriptors);
    }
}
