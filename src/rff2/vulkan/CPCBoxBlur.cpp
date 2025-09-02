//
// Created by Merutilm on 2025-08-28.
//

#include "CPCBoxBlur.hpp"

#include "SharedImageContextIndices.hpp"
#include "../../vulkan_helper/executor/ScopedCommandBufferExecutor.hpp"
#include "../../vulkan_helper/util/BarrierUtils.hpp"

namespace merutilm::rff2 {
    CPCBoxBlur::CPCBoxBlur(vkh::EngineRef engine) : ComputePipelineConfigurator(
        engine, "vk_box_blur.comp") {
    }

    void CPCBoxBlur::updateQueue(vkh::DescriptorUpdateQueue &queue, const uint32_t frameIndex) {
        //no operation
    }

    /**
     * Set Gaussian blur using 3x box blur.
     * @param srcImage the source image to blur. when the gaussian blur starts, its layout must be <b>VK_IMAGE_LAYOUT_GENERAL</b>.
     * after the operation, the layout will be changed to <b>VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL</b>.
     * it can be used in fragment shader without any layout transition.
     * @param dstImage the destination of blurred image. previous image is discarded,
     * and the layout will be changed to <b>VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL</b>.
     */
    void CPCBoxBlur::setGaussianBlur(const vkh::MultiframeImageContext &srcImage,
                                     const vkh::MultiframeImageContext &dstImage) {
        setExtent(srcImage[0].extent);
        setImages(0, srcImage, dstImage);
        setImages(1, dstImage, srcImage);
        setImages(2, srcImage, dstImage);
    }


    void CPCBoxBlur::cmdGaussianBlur(const uint32_t frameIndex, const uint32_t blurSizeDescIndex) {
        const VkCommandBuffer cbh = engine.getCommandBuffer().getCommandBufferHandle(frameIndex);
        auto &blurDesc = getDescriptor(SET_BLUR_IMAGE);

        auto ctxGetter = [&blurDesc, &frameIndex](const uint32_t descIndex, const uint32_t binding) {
            return blurDesc.get<vkh::StorageImage>(descIndex, binding).ctx[frameIndex];
        };
        const auto src = ctxGetter(0, BINDING_BLUR_IMAGE_SRC);
        const auto dst = ctxGetter(2, BINDING_BLUR_IMAGE_DST);

        vkh::BarrierUtils::cmdImageMemoryBarrier(cbh, dst.image, 0,
                                                        VK_ACCESS_SHADER_WRITE_BIT, VK_IMAGE_LAYOUT_UNDEFINED,
                                                        VK_IMAGE_LAYOUT_GENERAL, 0,
                                                        VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
                                                        VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT);

        cmdRender(cbh, frameIndex, {0u, blurSizeDescIndex});
        vkh::BarrierUtils::cmdMemoryBarrier(cbh, VK_ACCESS_SHADER_WRITE_BIT, VK_ACCESS_SHADER_READ_BIT, VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT, VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT);
        cmdRender(cbh, frameIndex, {1u, blurSizeDescIndex});
        vkh::BarrierUtils::cmdMemoryBarrier(cbh, VK_ACCESS_SHADER_WRITE_BIT, VK_ACCESS_SHADER_READ_BIT, VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT, VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT);
        cmdRender(cbh, frameIndex, {2u, blurSizeDescIndex});
        vkh::BarrierUtils::cmdImageMemoryBarrier(cbh, src.image,
                                                        VK_ACCESS_SHADER_READ_BIT,
                                                        VK_ACCESS_SHADER_READ_BIT, VK_IMAGE_LAYOUT_GENERAL,
                                                        VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, 0,
                                                        VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
                                                        VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT);
        vkh::BarrierUtils::cmdImageMemoryBarrier(cbh, dst.image,
                                                        VK_ACCESS_SHADER_WRITE_BIT,
                                                        VK_ACCESS_SHADER_READ_BIT, VK_IMAGE_LAYOUT_GENERAL,
                                                        VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, 0,
                                                        VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
                                                        VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT);
    }


    void CPCBoxBlur::initSize() const {
        auto &desc = getDescriptor(SET_BLUR_IMAGE);
        const uint32_t maxFramesInFlight = engine.getCore().getPhysicalDevice().getMaxFramesInFlight();
        for (uint32_t i = 0; i < BOX_BLUR_COUNT; ++i) {
            desc.get<vkh::StorageImage>(i, BINDING_BLUR_IMAGE_SRC).ctx = std::vector<vkh::ImageContext>(
                maxFramesInFlight);
            desc.get<vkh::StorageImage>(i, BINDING_BLUR_IMAGE_DST).ctx = std::vector<vkh::ImageContext>(
                maxFramesInFlight);
        }
    }

    void CPCBoxBlur::setImages(const uint32_t descIndex, const vkh::MultiframeImageContext &srcImage,
                               const vkh::MultiframeImageContext &dstImage) const {
        auto &desc = getDescriptor(SET_BLUR_IMAGE);


        writeDescriptorForEachFrame(
            [&desc, &srcImage, &dstImage, &descIndex](vkh::DescriptorUpdateQueue &queue, const uint32_t frameIndex) {
                desc.get<vkh::StorageImage>(descIndex, BINDING_BLUR_IMAGE_SRC).ctx[frameIndex] = srcImage[frameIndex];
                desc.get<vkh::StorageImage>(descIndex, BINDING_BLUR_IMAGE_DST).ctx[frameIndex] = dstImage[frameIndex];
                desc.queue(queue, frameIndex, {descIndex}, {BINDING_BLUR_IMAGE_SRC, BINDING_BLUR_IMAGE_DST});
            });
    }


    void CPCBoxBlur::setBlurSize(uint32_t blurSizeDescIndex, const float blurSize) const {
        auto &desc = getDescriptor(SET_BLUR_RADIUS);

        for (uint32_t i = 0; i < BLUR_TARGET_COUNT_PER_FRAME; ++i) {
            const auto &ubo = *desc.get<vkh::Uniform>(i, BINDING_BLUR_RADIUS_UBO);
            ubo.getHostObject().set<float>(TARGET_BLUR_UBO_BLUR_SIZE, blurSize);
            writeDescriptorForEachFrame(
                [&desc, &ubo, &blurSizeDescIndex](vkh::DescriptorUpdateQueue &queue, const uint32_t frameIndex) {
                    ubo.update(frameIndex);
                    desc.queue(queue, frameIndex, {blurSizeDescIndex}, {BINDING_BLUR_RADIUS_UBO});
                });
        }
    }

    void CPCBoxBlur::pipelineInitialized() {
        //no operation
    }

    void CPCBoxBlur::windowResized() {
        using namespace SharedImageContextIndices;
        setGaussianBlur(engine.getSharedImageContext().getMultiframeContext(MF_RENDER_DOWNSAMPLED_IMAGE_PRIMARY),
                        engine.getSharedImageContext().getMultiframeContext(MF_RENDER_DOWNSAMPLED_IMAGE_SECONDARY));
        //no operation
    }


    void CPCBoxBlur::configurePushConstant(vkh::PipelineLayoutManagerRef pipelineLayoutManager) {
        //no operation
    }

    void CPCBoxBlur::configureDescriptors(std::vector<vkh::DescriptorPtr> &descriptors) {
        auto imgDesc = std::vector<vkh::DescriptorManager>(BOX_BLUR_COUNT);
        for (uint32_t i = 0; i < BOX_BLUR_COUNT; ++i) {
            auto descManager = vkh::factory::create<vkh::DescriptorManager>();
            descManager->appendStorageImage(BINDING_BLUR_IMAGE_SRC, VK_SHADER_STAGE_COMPUTE_BIT);
            descManager->appendStorageImage(BINDING_BLUR_IMAGE_DST, VK_SHADER_STAGE_COMPUTE_BIT);
            imgDesc[i] = std::move(descManager);
        }

        auto radDesc = std::vector<vkh::DescriptorManager>(BLUR_TARGET_COUNT_PER_FRAME);
        for (uint32_t i = 0; i < BLUR_TARGET_COUNT_PER_FRAME; ++i) {
            auto descManager = vkh::factory::create<vkh::DescriptorManager>();
            auto bufferManager = vkh::factory::create<vkh::HostDataObjectManager>();
            bufferManager->reserve<float>(TARGET_BLUR_UBO_BLUR_SIZE);
            auto descUBO = vkh::factory::create<vkh::Uniform>(engine.getCore(), std::move(bufferManager),
                                                              vkh::BufferLock::LOCK_UNLOCK);
            descManager->appendUBO(BINDING_BLUR_RADIUS_UBO, VK_SHADER_STAGE_COMPUTE_BIT, std::move(descUBO));
            radDesc[i] = std::move(descManager);
        }

        appendUniqueDescriptor(SET_BLUR_IMAGE, descriptors, std::move(imgDesc));
        appendUniqueDescriptor(SET_BLUR_RADIUS, descriptors, std::move(radDesc));
    }
}
