//
// Created by Merutilm on 2025-07-29.
//

#include "../vulkan/GPCIterationPalette.hpp"

#include "SharedDescriptorTemplate.hpp"
#include "../../vulkan_helper/util/BufferImageContextUtils.hpp"
#include "../../vulkan_helper/util/DescriptorUpdater.hpp"
#include "../attr/ShdPaletteAttribute.h"
#include "../ui/Utilities.h"

namespace merutilm::rff2 {
    void GPCIterationPalette::updateQueue(vkh::DescriptorUpdateQueue &queue,
                                          const uint32_t frameIndex) {
        using namespace SharedDescriptorTemplate;
        auto &timeDesc = getDescriptor(SET_TIME);
        const auto &timeBinding = *timeDesc.get<vkh::Uniform>(0, DescTime::BINDING_UBO_TIME);

        timeBinding.getHostObject().set(DescTime::TARGET_TIME_CURRENT, Utilities::getCurrentTime());
        timeBinding.updateMF(frameIndex);
    }

    void GPCIterationPalette::cmdRefreshIterations(const VkCommandBuffer cbh, const vkh::BufferContext &src) const {
        vkh::BufferImageContextUtils::cmdCopyBuffer(cbh, src, getResultIterationBuffer());
    }

    const vkh::BufferContext &GPCIterationPalette::getResultIterationBuffer() const {
        using namespace SharedDescriptorTemplate;
        auto &iterDesc = getDescriptor(SET_ITERATION);
        const auto &iterSSBO = *iterDesc.get<vkh::ShaderStorage>(0,
                                                                 DescIteration::BINDING_SSBO_ITERATION_MATRIX);
        return iterSSBO.getBufferContext();
    }


    void GPCIterationPalette::resetIterationBuffer(const uint32_t width, const uint32_t height) {
        using namespace SharedDescriptorTemplate;
        auto &iterDesc = getDescriptor(SET_ITERATION);
        const auto &iterUBO = *iterDesc.get<vkh::Uniform>(0, DescIteration::BINDING_UBO_ITERATION_INFO);
        auto &iterUBOHost = iterUBO.getHostObject();
        auto &iterSSBO = *iterDesc.get<vkh::ShaderStorage>(0, DescIteration::BINDING_SSBO_ITERATION_MATRIX);
        auto &iterSSBOHost = iterSSBO.getHostObject();

        this->iterWidth = width;
        this->iterHeight = height;
        iterUBOHost.set<glm::uvec2>(DescIteration::TARGET_UBO_ITERATION_EXTENT, {width, height});
        iterUBO.update(DescIteration::TARGET_UBO_ITERATION_EXTENT);

        iterSSBOHost.resizeArray<double>(DescIteration::TARGET_SSBO_ITERATION_BUFFER, width * height);
        iterSSBO.reloadBuffer();
        iterSSBO.lock(wc.getCommandPool());
        writeDescriptorMF(
            [&iterDesc](vkh::DescriptorUpdateQueue &queue, const uint32_t frameIndex) {
                iterDesc.queue(queue, frameIndex, {}, {DescIteration::BINDING_SSBO_ITERATION_MATRIX});
            });
    }

    void GPCIterationPalette::setMaxIteration(const double maxIteration) const {
        using namespace SharedDescriptorTemplate;
        const auto &iterUBO = *getDescriptor(SET_ITERATION).get<vkh::Uniform>(0,
                                                                              DescIteration::BINDING_UBO_ITERATION_INFO);

        iterUBO.getHostObject().set<double>(DescIteration::TARGET_UBO_ITERATION_MAX, maxIteration);
        iterUBO.update();
    }

    void GPCIterationPalette::setPalette(const ShdPaletteAttribute &palette) const {
        using namespace SharedDescriptorTemplate;
        auto &paletteDesc = getDescriptor(SET_PALETTE);
        auto &paletteSSBO = *paletteDesc.get<vkh::ShaderStorage>(0,
                                                                 DescPalette::BINDING_SSBO_PALETTE);
        auto &paletteSSBOHost = paletteSSBO.getHostObject();

        if (paletteSSBO.isLocked()) {
            paletteSSBO.unlock(wc.getCommandPool());
        }

        const auto paletteLength = static_cast<uint32_t>(palette.colors.size());

        paletteSSBOHost.set<uint32_t>(DescPalette::TARGET_PALETTE_SIZE, paletteLength);
        paletteSSBOHost.set<float>(DescPalette::TARGET_PALETTE_INTERVAL, palette.iterationInterval);
        paletteSSBOHost.set<double>(DescPalette::TARGET_PALETTE_OFFSET, palette.offsetRatio);
        paletteSSBOHost.set<uint32_t>(DescPalette::TARGET_PALETTE_SMOOTHING,
                                      static_cast<uint32_t>(palette.colorSmoothing));
        paletteSSBOHost.set<float>(DescPalette::TARGET_PALETTE_ANIMATION_SPEED, palette.animationSpeed);
        paletteSSBOHost.resizeArray<glm::vec4>(DescPalette::TARGET_PALETTE_COLORS, paletteLength);
        paletteSSBOHost.set<glm::vec4>(DescPalette::TARGET_PALETTE_COLORS, palette.colors);
        paletteSSBO.reloadBuffer();
        paletteSSBO.update();
        paletteSSBO.lock(wc.getCommandPool());

        writeDescriptorMF(
            [&paletteDesc](vkh::DescriptorUpdateQueue &queue, const uint32_t frameIndex) {
                paletteDesc.queue(queue, frameIndex, {}, {DescPalette::BINDING_SSBO_PALETTE});
            });
    }

    void GPCIterationPalette::pipelineInitialized() {
        using namespace SharedDescriptorTemplate;
        const auto &timeDesc = getDescriptor(SET_TIME);
        const auto &iterDesc = getDescriptor(SET_ITERATION);
        writeDescriptorMF(
            [&timeDesc, &iterDesc](vkh::DescriptorUpdateQueue &queue, const uint32_t frameIndex) {
                timeDesc.queue(queue, frameIndex, {}, {DescTime::BINDING_UBO_TIME});
                iterDesc.queue(queue, frameIndex, {}, {DescIteration::BINDING_UBO_ITERATION_INFO});
            });
    }

    void GPCIterationPalette::renderContextRefreshed() {
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
