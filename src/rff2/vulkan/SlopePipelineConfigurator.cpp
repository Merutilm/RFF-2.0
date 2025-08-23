//
// Created by Merutilm on 2025-08-15.
//

#include "SlopePipelineConfigurator.hpp"

#include "RFFRenderContextConfigurator.hpp"
#include "SharedDescriptorTemplate.hpp"

namespace merutilm::rff2 {
    void SlopePipelineConfigurator::updateQueue(vkh::DescriptorUpdateQueue &queue, const uint32_t frameIndex,
                                                const uint32_t imageIndex, uint32_t width, uint32_t height) {

        auto &input = getRenderContextConfigurator<RFFRenderContextConfigurator>().getImageContext(
           RFFRenderContextConfigurator::SECONDARY_SUBPASS_RESULT_COLOR_IMAGE, imageIndex);
        auto &inputDesc = getDescriptor(SET_PREV_RESULT);
        auto &inputManager = inputDesc.getDescriptorManager();
        inputManager.get<vkh::ImageContext>(BINDING_PREV_RESULT_INPUT) = input;
        inputDesc.queue(queue, frameIndex);
    }

    void SlopePipelineConfigurator::setResolution(const glm::vec2 &swapchainExtent, const float clarityMultiplier) const {
        using namespace SharedDescriptorTemplate;
        auto &resDesc = getDescriptor(SET_RESOLUTION);
        auto &resManager = resDesc.getDescriptorManager();
        auto &resUBO = resManager.get<vkh::Uniform>(DescResolution::BINDING_UBO_RESOLUTION);
        auto &resUBOHost = resUBO->getHostObject();
        resUBOHost.set<glm::vec2>(DescResolution::TARGET_RESOLUTION_SWAPCHAIN_EXTENT, swapchainExtent);
        resUBOHost.set<float>(DescResolution::TARGET_RESOLUTION_CLARITY_MULTIPLIER, clarityMultiplier);

        writeDescriptorForEachFrame([&resUBO, &resDesc](vkh::DescriptorUpdateQueue &queue, const uint32_t frameIndex) {
            resUBO->update(frameIndex);
            resDesc.queue(queue, frameIndex);
        });
    }

    void SlopePipelineConfigurator::setSlope(const ShdSlopeAttribute &slope) const {
        using namespace SharedDescriptorTemplate;
        auto &slopeDesc = getDescriptor(SET_SLOPE);
        auto &slopeManager = slopeDesc.getDescriptorManager();
        auto &slopeUBO = slopeManager.get<vkh::Uniform>(DescSlope::BINDING_UBO_SLOPE);
        auto &slopeUBOHost = slopeUBO->getHostObject();
        slopeUBOHost.set<float>(DescSlope::TARGET_SLOPE_DEPTH, slope.depth);
        slopeUBOHost.set<float>(DescSlope::TARGET_SLOPE_REFLECTION_RATIO, slope.reflectionRatio);
        slopeUBOHost.set<float>(DescSlope::TARGET_SLOPE_OPACITY, slope.opacity);
        slopeUBOHost.set<float>(DescSlope::TARGET_SLOPE_ZENITH, slope.zenith);
        slopeUBOHost.set<float>(DescSlope::TARGET_SLOPE_AZIMUTH, slope.azimuth);

        writeDescriptorForEachFrame([&slopeUBO, &slopeDesc](vkh::DescriptorUpdateQueue &queue, const uint32_t frameIndex) {
            slopeUBO->update(frameIndex);
            slopeDesc.queue(queue, frameIndex);
        });
    }

    void SlopePipelineConfigurator::configurePushConstant(vkh::PipelineLayoutManagerRef pipelineLayoutManager) {
        //noop
    }

    void SlopePipelineConfigurator::configureDescriptors(std::vector<const vkh::Descriptor *> &descriptors) {
        using namespace SharedDescriptorTemplate;
        auto descManager = vkh::Factory::create<vkh::DescriptorManager>();
        descManager->appendInputAttachment(BINDING_PREV_RESULT_INPUT, VK_SHADER_STAGE_FRAGMENT_BIT);

        appendUniqueDescriptor(SET_PREV_RESULT, descriptors, std::move(descManager));
        appendDescriptor<DescIteration>(SET_ITERATION, descriptors);
        appendDescriptor<DescSlope>(SET_SLOPE, descriptors);
        appendDescriptor<DescResolution>(SET_RESOLUTION, descriptors);
    }
}
