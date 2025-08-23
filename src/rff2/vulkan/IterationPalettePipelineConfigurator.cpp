//
// Created by Merutilm on 2025-07-29.
//

#include "../vulkan/IterationPalettePipelineConfigurator.hpp"

#include "SharedDescriptorTemplate.hpp"
#include "../../vulkan_helper/util/DescriptorUpdater.hpp"
#include "../../vulkan_helper/util/ImageContextUtils.hpp"
#include "../data/Color.hpp"
#include "../attr/ShdPaletteAttribute.h"
#include "../ui/Utilities.h"

namespace merutilm::rff2 {

    void IterationPalettePipelineConfigurator::updateQueue(vkh::DescriptorUpdateQueue &queue, const uint32_t frameIndex,
                                                           const uint32_t imageIndex,
                                                           const uint32_t width, const uint32_t height) {
        using namespace SharedDescriptorTemplate;
        auto &iterDesc = getDescriptor(SET_ITERATION);
        const auto &iterSSBO = *iterDesc.getDescriptorManager().get<vkh::ShaderStorage> (
            DescIteration::BINDING_SSBO_ITERATION);
        iterSSBO.update(frameIndex);
        auto &timeDesc = getDescriptor(SET_TIME);
        const auto &timeBinding = *timeDesc.getDescriptorManager().get<vkh::Uniform>(DescTime::BINDING_UBO_TIME);
        timeBinding.getHostObject().set(DescTime::TARGET_TIME_CURRENT, Utilities::getCurrentTime());
        timeBinding.update(frameIndex);
        timeDesc.queue(queue, frameIndex);
    }

    void IterationPalettePipelineConfigurator::reloadIterationBuffer(const uint32_t width, const uint32_t height) {
        using namespace SharedDescriptorTemplate;
        auto &iterDesc = getDescriptor(SET_ITERATION);
        auto &iterSSBO = *iterDesc.getDescriptorManager().get<vkh::ShaderStorage> (
            DescIteration::BINDING_SSBO_ITERATION);
        auto &iterSSBOHost = iterSSBO.getHostObject();

        this->width = width;
        this->height = height;
        iterSSBOHost.set<glm::uvec2>(DescIteration::TARGET_ITERATION_EXTENT, {width, height});
        iterSSBOHost.resize<double>(DescIteration::TARGET_ITERATION_BUFFER, width * height);
        iterSSBO.reloadBuffer();
        vkh::DescriptorUpdateQueue updateQueue = vkh::DescriptorUpdater::createQueue();
        for (uint32_t i = 0; i < engine.getCore().getPhysicalDevice().getMaxFramesInFlight(); ++i) {
            iterSSBO.update(i, DescIteration::TARGET_ITERATION_EXTENT);
            iterDesc.queue(updateQueue, i);
        }

        vkh::DescriptorUpdater::write(engine.getCore().getLogicalDevice().getLogicalDeviceHandle(), updateQueue);
    }

    void IterationPalettePipelineConfigurator::resetIterationBuffer() const {
        using namespace SharedDescriptorTemplate;
        const auto &iterSSBO = *getDescriptor(SET_ITERATION).getDescriptorManager().get<vkh::ShaderStorage >(
            DescIteration::BINDING_SSBO_ITERATION);
        iterSSBO.getHostObject().reset(DescIteration::TARGET_ITERATION_BUFFER);
    }

    void IterationPalettePipelineConfigurator::setIteration(const uint32_t x, const uint32_t y,
                                                            double iteration) const {
        using namespace SharedDescriptorTemplate;
        const auto &iterSSBO = *getDescriptor(SET_ITERATION).getDescriptorManager().get<vkh::ShaderStorage >(
            DescIteration::BINDING_SSBO_ITERATION);
        iterSSBO.getHostObject().set<double>(DescIteration::TARGET_ITERATION_BUFFER, y * width + x, iteration);
    }

    void IterationPalettePipelineConfigurator::setAllIterations(const std::vector<double> &iterations) const {
        using namespace SharedDescriptorTemplate;
        const auto &iterSSBO = *getDescriptor(SET_ITERATION).getDescriptorManager().get<vkh::ShaderStorage >(
            DescIteration::BINDING_SSBO_ITERATION);

        iterSSBO.getHostObject().set<double>(DescIteration::TARGET_ITERATION_MAX, iterations);
    }

    void IterationPalettePipelineConfigurator::setMaxIteration(const double maxIteration) const {
        using namespace SharedDescriptorTemplate;
        const auto &iterSSBO = *getDescriptor(SET_ITERATION).getDescriptorManager().get<vkh::ShaderStorage >(
            DescIteration::BINDING_SSBO_ITERATION);

        iterSSBO.getHostObject().set<double>(DescIteration::TARGET_ITERATION_MAX, maxIteration);
    }

    void IterationPalettePipelineConfigurator::setPalette(const ShdPaletteAttribute &palette) const {
        using namespace SharedDescriptorTemplate;
        auto &paletteDesc = getDescriptor(SET_PALETTE);
        auto &paletteManager = paletteDesc.getDescriptorManager();
        auto &paletteSampler = *paletteManager.get<vkh::CombinedImageSampler>(
            DescPalette::BINDING_SAMPLER_PALETTE);
        const auto &paletteUBO = *paletteManager.get<vkh::Uniform >(
            DescPalette::BINDING_UBO_PALETTE);
        auto &paletteUBOHost = paletteUBO.getHostObject();
        const auto paletteLength = static_cast<uint32_t>(palette.colors.size());
        const uint32_t width = std::min(
            engine.getCore().getPhysicalDevice().getPhysicalDeviceProperties().limits.maxImageDimension2D,
            paletteLength);
        const uint32_t height = (paletteLength - 1) / width + 1;
        std::vector<Color> parsed(width * height * 4);
        std::ranges::transform(palette.colors, parsed.begin(), [](const NormalizedColor &color) {
            return Color::from(color);
        });

        auto paletteContext = vkh::ImageContextUtils::imageFromByteColorArray(
            engine.getCore(), engine.getCommandPool(), width, height, 4,
            reinterpret_cast<std::byte *>(parsed.data()));

        paletteUBOHost.set<glm::uvec2>(DescPalette::TARGET_PALETTE_EXTENT, {width, height});
        paletteUBOHost.set<uint32_t>(DescPalette::TARGET_PALETTE_SIZE, paletteLength);
        paletteUBOHost.set<float>(DescPalette::TARGET_PALETTE_INTERVAL, palette.iterationInterval);
        paletteUBOHost.set<double>(DescPalette::TARGET_PALETTE_OFFSET, palette.offsetRatio);
        paletteUBOHost.set<uint32_t>(DescPalette::TARGET_PALETTE_SMOOTHING,
                                     static_cast<uint32_t>(palette.colorSmoothing));
        paletteUBOHost.set<float>(DescPalette::TARGET_PALETTE_ANIMATION_SPEED, palette.animationSpeed);
        paletteSampler.setUniqueImageContext(std::move(paletteContext));

        writeDescriptorForEachFrame([&paletteUBO, &paletteDesc](vkh::DescriptorUpdateQueue &queue, const uint32_t frameIndex) {
            paletteUBO.update(frameIndex);
            paletteDesc.queue(queue, frameIndex);
        });
    }


    void IterationPalettePipelineConfigurator::configurePushConstant(
        vkh::PipelineLayoutManagerRef pipelineLayoutManager) {
        //noop
    }

    void IterationPalettePipelineConfigurator::configureDescriptors(std::vector<const vkh::Descriptor *> &descriptors) {
        using namespace SharedDescriptorTemplate;
        appendDescriptor<DescIteration>(SET_ITERATION, descriptors);
        appendDescriptor<DescPalette>(SET_PALETTE, descriptors);
        appendDescriptor<DescTime>(SET_TIME, descriptors);
    }
}
