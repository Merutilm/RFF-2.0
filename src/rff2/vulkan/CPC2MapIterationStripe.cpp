//
// Created by Merutilm on 2025-09-06.
//

#include "CPC2MapIterationStripe.hpp"

#include "SharedDescriptorTemplate.hpp"
#include "SharedImageContextIndices.hpp"
#include "../attr/ShdPaletteAttribute.h"

namespace merutilm::rff2 {
    void CPC2MapIterationStripe::updateQueue(vkh::DescriptorUpdateQueue &queue, uint32_t frameIndex) {
        //noop
    }


    void CPC2MapIterationStripe::pipelineInitialized() {
        using namespace SharedDescriptorTemplate;
        const auto &iterDesc = getDescriptor(SET_OUTPUT_ITERATION);
        const auto &stripeDesc = getDescriptor(SET_STRIPE);
        const auto &timeDesc = getDescriptor(SET_TIME);
        const auto &vidDesc = getDescriptor(SET_VIDEO);
        writeDescriptorMF([&iterDesc, &stripeDesc, &timeDesc, &vidDesc](vkh::DescriptorUpdateQueue &queue, const uint32_t frameIndex) {
            iterDesc.queue(queue, frameIndex, {}, {DescIteration::BINDING_UBO_ITERATION_INFO});
            stripeDesc.queue(queue, frameIndex, {}, {DescStripe::BINDING_UBO_STRIPE});
            timeDesc.queue(queue, frameIndex, {}, {DescTime::BINDING_UBO_TIME});
            vidDesc.queue(queue, frameIndex, {}, {DescVideo::BINDING_UBO_VIDEO});
        });
    }

    void CPC2MapIterationStripe::renderContextRefreshed() {
        using namespace SharedImageContextIndices;
        auto &outDesc = getDescriptor(SET_OUTPUT_IMAGE);
        auto &[outImg] = outDesc.get<vkh::StorageImage>(0, BINDING_OUTPUT_MERGED_IMAGE);
        outImg = wc.getSharedImageContext().getImageContextMF(MF_VIDEO_RENDER_IMAGE_PRIMARY);
        writeDescriptorMF(
            [&outDesc](vkh::DescriptorUpdateQueue &queue, const uint32_t frameIndex) {
                outDesc.queue(queue, frameIndex, {}, {BINDING_OUTPUT_MERGED_IMAGE});
            });
    }


    void CPC2MapIterationStripe::setCurrentFrame(const float currentFrame, const uint32_t frameIndex) const {
        using namespace SharedDescriptorTemplate;
        auto &vidDesc = getDescriptor(SET_VIDEO);
        const auto &vidUBO = *vidDesc.get<vkh::Uniform>(0, DescVideo::BINDING_UBO_VIDEO);
        auto &vidUBOHost = vidUBO.getHostObject();
        vidUBOHost.set<float>(DescVideo::TARGET_VIDEO_CURRENT_FRAME, currentFrame);
        vidUBO.updateMF(frameIndex);
    }


    void CPC2MapIterationStripe::setPalette(const ShdPaletteAttribute &palette) const {
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

    void CPC2MapIterationStripe::setStripe(const ShdStripeAttribute &stripe) const {
        using namespace SharedDescriptorTemplate;
        auto &stripeDesc = getDescriptor(SET_STRIPE);
        const auto &stripeUBO = *stripeDesc.get<vkh::Uniform>(0, DescStripe::BINDING_UBO_STRIPE);
        auto &stripeUBOHost = stripeUBO.getHostObject();
        stripeUBOHost.set(DescStripe::TARGET_STRIPE_TYPE, static_cast<uint32_t>(stripe.stripeType));
        stripeUBOHost.set(DescStripe::TARGET_STRIPE_FIRST_INTERVAL,
                          stripe.firstInterval);
        stripeUBOHost.set(DescStripe::TARGET_STRIPE_SECOND_INTERVAL,
                          stripe.secondInterval);
        stripeUBOHost.set(DescStripe::TARGET_STRIPE_OPACITY, stripe.opacity);
        stripeUBOHost.set(DescStripe::TARGET_STRIPE_OFFSET, stripe.offset);
        stripeUBOHost.set(DescStripe::TARGET_STRIPE_ANIMATION_SPEED,
                          stripe.animationSpeed);
        stripeUBO.update();
    }



    void CPC2MapIterationStripe::setDefaultZoomIncrement(const float defaultZoomIncrement) const {
        using namespace SharedDescriptorTemplate;
        auto &vidDesc = getDescriptor(SET_VIDEO);
        const auto &vidUBO = *vidDesc.get<vkh::Uniform>(0, DescVideo::BINDING_UBO_VIDEO);
        auto &vidUBOHost = vidUBO.getHostObject();
        vidUBOHost.set<float>(DescVideo::TARGET_VIDEO_DEFAULT_ZOOM_INCREMENT, defaultZoomIncrement);
        updateBufferMF([&vidUBO](const uint32_t frameIndex) {
            vidUBO.updateMF(frameIndex);
        });
    }


    void CPC2MapIterationStripe::setAllIterations(const std::vector<double> &normal,
                                                   const std::vector<double> &zoomed) const {
        using namespace SharedDescriptorTemplate;
        auto &map2Desc = getDescriptor(SET_I2MAP);
        const auto &map2DescNormalSSBO = *map2Desc.get<vkh::ShaderStorage>(0, BINDING_I2MAP_SSBO_NORMAL);
        map2DescNormalSSBO.getHostObject().set<double>(
            TARGET_I2MAP_SSBO_NORMAL_ITERATION, normal);
        const auto &map2DescZoomedSSBO = *map2Desc.get<vkh::ShaderStorage>(0, BINDING_I2MAP_SSBO_ZOOMED);
        map2DescZoomedSSBO.getHostObject().set<double>(
            TARGET_I2MAP_SSBO_ZOOMED_ITERATION, zoomed);

        map2DescNormalSSBO.update();
        map2DescZoomedSSBO.update();
    }

    void CPC2MapIterationStripe::set2MapSize(const VkExtent2D &extent) {
        using namespace SharedDescriptorTemplate;
        const auto &[width, height] = extent;
        setExtent(extent);
        auto &iter = getDescriptor(SET_I2MAP);
        auto &iterNormalSSBO = *iter.get<vkh::ShaderStorage>(0, BINDING_I2MAP_SSBO_NORMAL);
        iterNormalSSBO.getHostObject().resizeAndClear<double>(TARGET_I2MAP_SSBO_NORMAL_ITERATION, width * height);
        iterNormalSSBO.reloadBuffer();

        auto &iterZoomedSSBO = *iter.get<vkh::ShaderStorage>(0, BINDING_I2MAP_SSBO_ZOOMED);
        iterZoomedSSBO.getHostObject().resizeAndClear<double>(TARGET_I2MAP_SSBO_ZOOMED_ITERATION, width * height);
        iterZoomedSSBO.reloadBuffer();

        auto &iterOut = getDescriptor(SET_OUTPUT_ITERATION);
        auto &iterOutSSBO = *iterOut.get<vkh::ShaderStorage>(0, DescIteration::BINDING_SSBO_ITERATION_MATRIX);
        if (iterOutSSBO.isLocked()) {
            iterOutSSBO.unlock(wc.getCommandPool());
        }
        iterOutSSBO.getHostObject().resizeAndClear<double>(DescIteration::TARGET_SSBO_ITERATION_BUFFER, width * height);
        iterOutSSBO.reloadBuffer();
        iterOutSSBO.lock(wc.getCommandPool());

        const auto &iterOutUBO = *iterOut.get<vkh::Uniform>(0, DescIteration::BINDING_UBO_ITERATION_INFO);
        iterOutUBO.getHostObject().set<glm::uvec2>(DescIteration::TARGET_UBO_ITERATION_EXTENT, {width, height});
        iterOutUBO.update(DescIteration::TARGET_UBO_ITERATION_EXTENT);


        writeDescriptorMF([&iter, &iterOut](vkh::DescriptorUpdateQueue &queue, const uint32_t frameIndex) {
            iter.queue(queue, frameIndex, {}, {BINDING_I2MAP_SSBO_NORMAL, BINDING_I2MAP_SSBO_ZOOMED});
            iterOut.queue(queue, frameIndex, {}, {DescIteration::BINDING_UBO_ITERATION_INFO, DescIteration::BINDING_SSBO_ITERATION_MATRIX});
        });
    }


    void CPC2MapIterationStripe::setInfo(const double maxIteration) const {
        using namespace SharedDescriptorTemplate;

        auto &iter = getDescriptor(SET_OUTPUT_ITERATION);
        const auto &iterOutUBO = *iter.get<vkh::Uniform>(0, DescIteration::BINDING_UBO_ITERATION_INFO);
        iterOutUBO.getHostObject().set<double>(DescIteration::TARGET_UBO_ITERATION_MAX, maxIteration);
        iterOutUBO.update(DescIteration::TARGET_UBO_ITERATION_MAX);
    }

    void CPC2MapIterationStripe::setTime(const float currentSec, const uint32_t frameIndex) const {
        using namespace SharedDescriptorTemplate;
        auto &time = getDescriptor(SET_TIME);
        const auto &timeUBO = *time.get<vkh::Uniform>(0, DescTime::BINDING_UBO_TIME);
        timeUBO.getHostObject().set<float>(DescTime::TARGET_TIME_CURRENT, currentSec);
        timeUBO.updateMF(frameIndex, DescTime::TARGET_TIME_CURRENT);

    }

    void CPC2MapIterationStripe::configurePushConstant(vkh::PipelineLayoutManagerRef pipelineLayoutManager) {
        //noop
    }

    void CPC2MapIterationStripe::configureDescriptors(std::vector<vkh::DescriptorPtr> &descriptors) {
        using namespace SharedDescriptorTemplate;
        auto normal = vkh::factory::create<vkh::HostDataObjectManager>();
        normal->reserveArray<double>(TARGET_I2MAP_SSBO_NORMAL_ITERATION, 1);
        auto normalSSBO = vkh::factory::create<vkh::ShaderStorage>(wc.core, std::move(normal),
                                                                   vkh::BufferLock::ALWAYS_MUTABLE, false);
        auto zoomed = vkh::factory::create<vkh::HostDataObjectManager>();
        zoomed->reserveArray<double>(TARGET_I2MAP_SSBO_ZOOMED_ITERATION, 1);
        auto zoomedSSBO = vkh::factory::create<vkh::ShaderStorage>(wc.core, std::move(zoomed),
                                                                   vkh::BufferLock::ALWAYS_MUTABLE, false);

        auto i2mapManager = vkh::factory::create<vkh::DescriptorManager>();
        i2mapManager->appendSSBO(BINDING_I2MAP_SSBO_NORMAL, VK_SHADER_STAGE_COMPUTE_BIT, std::move(normalSSBO));
        i2mapManager->appendSSBO(BINDING_I2MAP_SSBO_ZOOMED, VK_SHADER_STAGE_COMPUTE_BIT, std::move(zoomedSSBO));
        appendUniqueDescriptor(SET_I2MAP, descriptors, std::move(i2mapManager));
        appendDescriptor<DescVideo>(SET_VIDEO, descriptors);
        appendDescriptor<DescPalette>(SET_PALETTE, descriptors);
        appendDescriptor<DescTime>(SET_TIME, descriptors);

        auto outputManager = vkh::factory::create<vkh::DescriptorManager>();
        outputManager->appendStorageImage(BINDING_OUTPUT_MERGED_IMAGE, VK_SHADER_STAGE_COMPUTE_BIT);
        appendUniqueDescriptor(SET_OUTPUT_IMAGE, descriptors, std::move(outputManager));
        appendDescriptor<DescIteration>(SET_OUTPUT_ITERATION, descriptors);
        appendDescriptor<DescStripe>(SET_STRIPE, descriptors);
    }
}
