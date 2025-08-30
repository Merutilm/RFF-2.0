//
// Created by Merutilm on 2025-07-29.
//

#include "../vulkan/GPCIterationPalette.hpp"

#include "SharedDescriptorTemplate.hpp"
#include "../../vulkan_helper/util/DescriptorUpdater.hpp"
#include "../attr/ShdPaletteAttribute.h"
#include "../ui/Utilities.h"

namespace merutilm::rff2 {
    void GPCIterationPalette::updateQueue(vkh::DescriptorUpdateQueue &queue,
                                                           const uint32_t frameIndex) {
        using namespace SharedDescriptorTemplate;
        const auto &iterDesc = getDescriptor(SET_ITERATION);
        const auto &iterSSBO = *iterDesc.getDescriptorManager().get<vkh::ShaderStorage>(
            DescIteration::BINDING_SSBO_ITERATION);

        const auto &timeDesc = getDescriptor(SET_TIME);
        const auto &timeBinding = *timeDesc.getDescriptorManager().get<vkh::Uniform>(DescTime::BINDING_UBO_TIME);

        iterSSBO.update(frameIndex);
        timeBinding.getHostObject().set(DescTime::TARGET_TIME_CURRENT, Utilities::getCurrentTime());
        timeBinding.update(frameIndex);
    }

    void GPCIterationPalette::resetIterationBuffer(const uint32_t width, const uint32_t height) {
        using namespace SharedDescriptorTemplate;
        const auto &iterDesc = getDescriptor(SET_ITERATION);
        auto &iterSSBO = *iterDesc.getDescriptorManager().get<vkh::ShaderStorage>(
            DescIteration::BINDING_SSBO_ITERATION);
        auto &iterSSBOHost = iterSSBO.getHostObject();

        this->iterWidth = width;
        this->iterHeight = height;
        iterSSBOHost.set<glm::uvec2>(DescIteration::TARGET_ITERATION_EXTENT, {width, height});
        iterSSBOHost.resize<double>(DescIteration::TARGET_ITERATION_BUFFER, width * height);
        iterSSBO.reloadBuffer();

        writeDescriptorForEachFrame(
            [&iterDesc, &iterSSBO](vkh::DescriptorUpdateQueue &queue, const uint32_t frameIndex) {
                iterSSBO.update(frameIndex, DescIteration::TARGET_ITERATION_EXTENT);
                iterDesc.queue(queue, frameIndex, {DescIteration::BINDING_SSBO_ITERATION});
            });
    }

    void GPCIterationPalette::resetIterationBuffer() const {
        using namespace SharedDescriptorTemplate;
        const auto &iterSSBO = *getDescriptor(SET_ITERATION).getDescriptorManager().get<vkh::ShaderStorage>(
            DescIteration::BINDING_SSBO_ITERATION);
        iterSSBO.getHostObject().reset(DescIteration::TARGET_ITERATION_BUFFER);
    }

    void GPCIterationPalette::setIteration(const uint32_t x, const uint32_t y,
                                                            double iteration) const {
        using namespace SharedDescriptorTemplate;
        const auto &iterSSBO = *getDescriptor(SET_ITERATION).getDescriptorManager().get<vkh::ShaderStorage>(
            DescIteration::BINDING_SSBO_ITERATION);
        iterSSBO.getHostObject().set<double>(DescIteration::TARGET_ITERATION_BUFFER, y * iterWidth + x, iteration);
    }

    void GPCIterationPalette::setAllIterations(const std::vector<double> &iterations) const {
        using namespace SharedDescriptorTemplate;
        const auto &iterSSBO = *getDescriptor(SET_ITERATION).getDescriptorManager().get<vkh::ShaderStorage>(
            DescIteration::BINDING_SSBO_ITERATION);

        iterSSBO.getHostObject().set<double>(DescIteration::TARGET_ITERATION_MAX, iterations);
    }

    void GPCIterationPalette::setMaxIteration(const double maxIteration) const {
        using namespace SharedDescriptorTemplate;
        const auto &iterSSBO = *getDescriptor(SET_ITERATION).getDescriptorManager().get<vkh::ShaderStorage>(
            DescIteration::BINDING_SSBO_ITERATION);

        iterSSBO.getHostObject().set<double>(DescIteration::TARGET_ITERATION_MAX, maxIteration);
    }

    void GPCIterationPalette::setPalette(const ShdPaletteAttribute &palette) const {
        using namespace SharedDescriptorTemplate;
        const auto &paletteDesc = getDescriptor(SET_PALETTE);
        auto &paletteManager = paletteDesc.getDescriptorManager();
        auto &paletteSSBO = *paletteManager.get<vkh::ShaderStorage>(
            DescPalette::BINDING_SSBO_PALETTE);
        auto &paletteSSBOHost = paletteSSBO.getHostObject();

        const auto paletteLength = static_cast<uint32_t>(palette.colors.size());

        paletteSSBOHost.set<uint32_t>(DescPalette::TARGET_PALETTE_SIZE, paletteLength);
        paletteSSBOHost.set<float>(DescPalette::TARGET_PALETTE_INTERVAL, palette.iterationInterval);
        paletteSSBOHost.set<double>(DescPalette::TARGET_PALETTE_OFFSET, palette.offsetRatio);
        paletteSSBOHost.set<uint32_t>(DescPalette::TARGET_PALETTE_SMOOTHING,
                                      static_cast<uint32_t>(palette.colorSmoothing));
        paletteSSBOHost.set<float>(DescPalette::TARGET_PALETTE_ANIMATION_SPEED, palette.animationSpeed);
        paletteSSBOHost.resize<glm::vec4>(DescPalette::TARGET_PALETTE_COLORS, paletteLength);
        paletteSSBOHost.set<glm::vec4>(DescPalette::TARGET_PALETTE_COLORS, palette.colors);
        paletteSSBO.reloadBuffer();

        writeDescriptorForEachFrame(
            [&paletteSSBO, &paletteDesc](vkh::DescriptorUpdateQueue &queue, const uint32_t frameIndex) {
                paletteSSBO.update(frameIndex);
                paletteDesc.queue(queue, frameIndex, {DescPalette::BINDING_SSBO_PALETTE});
            });
    }

    void GPCIterationPalette::pipelineInitialized() {
        using namespace SharedDescriptorTemplate;
        const auto &timeDesc = getDescriptor(SET_TIME);

        writeDescriptorForEachFrame(
            [&timeDesc](vkh::DescriptorUpdateQueue &queue, const uint32_t frameIndex) {
                timeDesc.queue(queue, frameIndex);
            });
    }

    void GPCIterationPalette::windowResized() {
        //no operation
    }


    void GPCIterationPalette::configurePushConstant(
        vkh::PipelineLayoutManagerRef pipelineLayoutManager) {
        //noop
    }

    void GPCIterationPalette::configureDescriptors(std::vector<vkh::DescriptorPtr> &descriptors) {
        using namespace SharedDescriptorTemplate;
        appendDescriptor<DescIteration>(SET_ITERATION, descriptors);
        appendDescriptor<DescPalette>(SET_PALETTE, descriptors);
        appendDescriptor<DescTime>(SET_TIME, descriptors);
    }
}
