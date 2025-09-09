//
// Created by Merutilm on 2025-08-28.
//

#include "CPCBoxBlur.hpp"

#include "SharedImageContextIndices.hpp"
#include "../../vulkan_helper/executor/ScopedCommandBufferExecutor.hpp"
#include "../../vulkan_helper/util/BarrierUtils.hpp"
#include "../constants/VulkanWindowConstants.hpp"

namespace merutilm::rff2 {
    void CPCBoxBlur::updateQueue(vkh::DescriptorUpdateQueue &queue, const uint32_t frameIndex) {
        //no operation
    }

    /**
     * Set Gaussian blur using 3x box blur.
     * @param srcImage the source image to blur. when the gaussian blur starts, its layout must be <b>VK_IMAGE_LAYOUT_GENERAL</b>.
     * it can be used in fragment shader without any layout transition.
     * @param dstImage the destination of blurred image. previous image is discarded.
     */
    void CPCBoxBlur::setGaussianBlur(const vkh::MultiframeImageContext &srcImage,
                                     const vkh::MultiframeImageContext &dstImage) {
        setExtent(srcImage[0].extent);
        setImages(0, srcImage, dstImage);
        setImages(1, dstImage, srcImage);
        setImages(2, srcImage, dstImage);
    }


    void CPCBoxBlur::cmdGaussianBlur(const uint32_t frameIndex, const uint32_t blurSizeDescIndex) {
        const VkCommandBuffer cbh = wc.getCommandBuffer().getCommandBufferHandle(frameIndex);
        auto &blurDesc = getDescriptor(SET_BLUR_IMAGE);

        auto ctxGetter = [&blurDesc, &frameIndex](const uint32_t descIndex, const uint32_t binding) {
            return blurDesc.get<vkh::StorageImage>(descIndex, binding).ctx[frameIndex];
        };

        const auto dst = ctxGetter(2, BINDING_BLUR_IMAGE_DST);

        vkh::BarrierUtils::cmdImageMemoryBarrier(cbh, dst.image, 0,
                                                 VK_ACCESS_SHADER_WRITE_BIT, VK_IMAGE_LAYOUT_UNDEFINED,
                                                 VK_IMAGE_LAYOUT_GENERAL, 0, 1,
                                                 VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
                                                 VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT);

        cmdRender(cbh, frameIndex, {0u, blurSizeDescIndex});
        vkh::BarrierUtils::cmdSynchronizeImageWriteToRead(cbh, dst.image, VK_IMAGE_LAYOUT_GENERAL, 0, 1,
                                                          VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
                                                          VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT);
        cmdRender(cbh, frameIndex, {1u, blurSizeDescIndex});
        vkh::BarrierUtils::cmdSynchronizeImageWriteToRead(cbh, dst.image, VK_IMAGE_LAYOUT_GENERAL, 0, 1,
                                                          VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
                                                          VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT);
        cmdRender(cbh, frameIndex, {2u, blurSizeDescIndex});

    }


    void CPCBoxBlur::initSize() const {
        auto &desc = getDescriptor(SET_BLUR_IMAGE);
        const uint32_t maxFramesInFlight = wc.core.getPhysicalDevice().getMaxFramesInFlight();
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


        writeDescriptorMF(
            [&desc, &srcImage, &dstImage, &descIndex](vkh::DescriptorUpdateQueue &queue, const uint32_t frameIndex) {
                desc.get<vkh::StorageImage>(descIndex, BINDING_BLUR_IMAGE_SRC).ctx[frameIndex] = srcImage[frameIndex];
                desc.get<vkh::StorageImage>(descIndex, BINDING_BLUR_IMAGE_DST).ctx[frameIndex] = dstImage[frameIndex];
                desc.queue(queue, frameIndex, {descIndex}, {BINDING_BLUR_IMAGE_SRC, BINDING_BLUR_IMAGE_DST});
            });
    }


    void CPCBoxBlur::setBlurInfo(uint32_t blurSizeDescIndex, const float blurSize) const {
        auto &desc = getDescriptor(SET_BLUR_RADIUS);

        const auto &ubo = *desc.get<vkh::Uniform>(blurSizeDescIndex, BINDING_BLUR_RADIUS_UBO);
        ubo.getHostObject().set<float>(TARGET_BLUR_UBO_BLUR_SIZE, blurSize);
        ubo.update();

        writeDescriptorMF(
            [&desc, &blurSizeDescIndex](vkh::DescriptorUpdateQueue &queue, const uint32_t frameIndex) {
                desc.queue(queue, frameIndex, {blurSizeDescIndex}, {BINDING_BLUR_RADIUS_UBO});
            });
    }

    void CPCBoxBlur::pipelineInitialized() {
        //no operation
    }

    void CPCBoxBlur::renderContextRefreshed() {
        using namespace SharedImageContextIndices;
        auto &sic = wc.getSharedImageContext();
        switch (wc.getAttachmentIndex()) {
            case Constants::VulkanWindow::MAIN_WINDOW_ATTACHMENT_INDEX: {
                setGaussianBlur(sic.getImageContextMF(MF_MAIN_RENDER_DOWNSAMPLED_IMAGE_PRIMARY),
                                sic.getImageContextMF(MF_MAIN_RENDER_DOWNSAMPLED_IMAGE_SECONDARY));
                break;
            }
            case Constants::VulkanWindow::VIDEO_WINDOW_ATTACHMENT_INDEX: {
                setGaussianBlur(sic.getImageContextMF(MF_VIDEO_RENDER_DOWNSAMPLED_IMAGE_PRIMARY),
                                sic.getImageContextMF(MF_VIDEO_RENDER_DOWNSAMPLED_IMAGE_SECONDARY));
                break;
            }
            default: {
                //noop
            }
        }
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

        auto radDesc = std::vector<vkh::DescriptorManager>(DESC_COUNT_BLUR_TARGET);
        for (uint32_t i = 0; i < DESC_COUNT_BLUR_TARGET; ++i) {
            auto descManager = vkh::factory::create<vkh::DescriptorManager>();
            auto bufferManager = vkh::factory::create<vkh::HostDataObjectManager>();
            bufferManager->reserve<float>(TARGET_BLUR_UBO_BLUR_SIZE);
            auto descUBO = vkh::factory::create<vkh::Uniform>(wc.core, std::move(bufferManager),
                                                              vkh::BufferLock::LOCK_UNLOCK, false);
            descManager->appendUBO(BINDING_BLUR_RADIUS_UBO, VK_SHADER_STAGE_COMPUTE_BIT, std::move(descUBO));
            radDesc[i] = std::move(descManager);
        }

        appendUniqueDescriptor(SET_BLUR_IMAGE, descriptors, std::move(imgDesc));
        appendUniqueDescriptor(SET_BLUR_RADIUS, descriptors, std::move(radDesc));
    }
}
