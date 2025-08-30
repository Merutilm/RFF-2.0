//
// Created by Merutilm on 2025-08-28.
//

#include "CPCBoxBlur.hpp"

#include "../../vulkan_helper/executor/ScopedSyncedCommandBufferExecutorForRenderPass.hpp"
#include "../../vulkan_helper/struct/RenderPassProcess.hpp"
#include "../../vulkan_helper/util/ImageContextUtils.hpp"

namespace merutilm::rff2 {
    CPCBoxBlur::CPCBoxBlur(vkh::EngineRef engine) : ComputePipelineConfigurator(
        engine, "vk_box_blur.comp") {
    }

    void CPCBoxBlur::updateQueue(vkh::DescriptorUpdateQueue &queue, const uint32_t frameIndex) {
        //no operation
    }

    /**
     * Gaussian blur using 3x box blur.
     * @param blurSize normalized blur size ratio (0 ~ 1)
     * @param srcImage the source image to blur. its layout must be <b>VK_IMAGE_LAYOUT_GENERAL</b>.
     * after the operation, the layout will be changed to <b>VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL</b>.
     * it can be used in fragment shader without any layout transition.
     * @param dstImage the destination of blurred image. previous image is discarded,
     * and the layout will be changed to <b>VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL</b>.
     * @param tempImage the temporary image for ping-pong operation.
     * @param frameIndex current Frame index
     */
    void CPCBoxBlur::gaussianBlur(const float blurSize, const vkh::ImageContext &srcImage,
                                                   const vkh::ImageContext &dstImage,
                                                   const vkh::ImageContext &tempImage, const uint32_t frameIndex) {
        using enum vkh::RenderPassProcessTypeFlagBits;
        const VkCommandBuffer cbh = engine.getCommandBuffer().getCommandBufferHandle(frameIndex);

        setExtent(srcImage.extent);
        setBlurSize(frameIndex, blurSize);
        setImages(frameIndex, srcImage, dstImage);
        {
            vkh::ScopedSyncedCommandBufferExecutorForRenderPass executor(engine, frameIndex, MIDDLE);
            vkh::ImageContextUtils::cmdTransformImageLayout(cbh,
                                                            tempImage, 0,
                                                            VK_ACCESS_SHADER_WRITE_BIT, VK_IMAGE_LAYOUT_UNDEFINED,
                                                            VK_IMAGE_LAYOUT_GENERAL, 0,
                                                            VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
                                                            VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT);
            vkh::ImageContextUtils::cmdTransformImageLayout(cbh,
                                                            dstImage, 0,
                                                            VK_ACCESS_SHADER_WRITE_BIT, VK_IMAGE_LAYOUT_UNDEFINED,
                                                            VK_IMAGE_LAYOUT_GENERAL, 0,
                                                            VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
                                                            VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT);
            cmdRender(cbh, frameIndex);
        }
        setImages(frameIndex, dstImage, tempImage); {
            vkh::ScopedSyncedCommandBufferExecutorForRenderPass executor(engine, frameIndex, MIDDLE);
            cmdRender(cbh, frameIndex);
        }
        setImages(frameIndex, tempImage, dstImage); {
            vkh::ScopedSyncedCommandBufferExecutorForRenderPass executor(engine, frameIndex, MIDDLE);
            cmdRender(cbh, frameIndex);
            vkh::ImageContextUtils::cmdTransformImageLayout(cbh,
                                                            srcImage,
                                                            VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_SHADER_WRITE_BIT,
                                                            VK_ACCESS_SHADER_READ_BIT, VK_IMAGE_LAYOUT_GENERAL,
                                                            VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, 0,
                                                            VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
                                                            VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT);
            vkh::ImageContextUtils::cmdTransformImageLayout(cbh,
                                                            dstImage,
                                                            VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_SHADER_WRITE_BIT,
                                                            VK_ACCESS_SHADER_READ_BIT, VK_IMAGE_LAYOUT_GENERAL,
                                                            VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, 0,
                                                            VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
                                                            VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT);
        }
    }


    void CPCBoxBlur::initSize() const {
        const auto &desc = getDescriptor(SET_BLUR);
        auto &descManager = desc.getDescriptorManager();
        const uint32_t maxFramesInFlight = engine.getCore().getPhysicalDevice().getMaxFramesInFlight();
        descManager.get<vkh::StorageImage>(BINDING_BLUR_SRC).ctx = std::vector<vkh::ImageContext>(maxFramesInFlight);
        descManager.get<vkh::StorageImage>(BINDING_BLUR_DST).ctx = std::vector<vkh::ImageContext>(maxFramesInFlight);
    }

    void CPCBoxBlur::setImages(const uint32_t frameIndex,
                                                const vkh::ImageContext &srcImage,
                                                const vkh::ImageContext &dstImage) const {
        const auto &desc = getDescriptor(SET_BLUR);
        auto &descManager = desc.getDescriptorManager();
        descManager.get<vkh::StorageImage>(BINDING_BLUR_SRC).ctx[frameIndex] = srcImage;
        descManager.get<vkh::StorageImage>(BINDING_BLUR_DST).ctx[frameIndex] = dstImage;

        writeDescriptor([&desc, &frameIndex](vkh::DescriptorUpdateQueue &queue) {
            desc.queue(queue, frameIndex, {BINDING_BLUR_SRC, BINDING_BLUR_DST});
        });
    }


    void CPCBoxBlur::setBlurSize(const uint32_t frameIndex,
                                                  const float blurSize) const {
        const auto &desc = getDescriptor(SET_BLUR);
        const auto &ubo = *desc.getDescriptorManager().get<vkh::Uniform>(BINDING_BLUR_UBO);
        ubo.getHostObject().set<float>(TARGET_BLUR_UBO_BLUR_SIZE, blurSize);

        writeDescriptor([&desc, &ubo, &frameIndex](vkh::DescriptorUpdateQueue &queue) {
            ubo.update(frameIndex);
            desc.queue(queue, frameIndex, {BINDING_BLUR_UBO});
        });
    }

    void CPCBoxBlur::pipelineInitialized() {
        //no operation
    }

    void CPCBoxBlur::windowResized() {
        //no operation
    }


    void CPCBoxBlur::configurePushConstant(vkh::PipelineLayoutManagerRef pipelineLayoutManager) {
        //no operation
    }

    void CPCBoxBlur::configureDescriptors(std::vector<vkh::DescriptorPtr> &descriptors) {
        auto descManager = vkh::Factory::create<vkh::DescriptorManager>();
        auto bufferManager = vkh::Factory::create<vkh::HostBufferObjectManager>();
        bufferManager->reserve<float>(TARGET_BLUR_UBO_BLUR_SIZE);
        auto descUBO = vkh::Factory::create<vkh::Uniform>(engine.getCore(), std::move(bufferManager),
                                                          vkh::BufferLock::LOCK_UNLOCK);
        descManager->appendStorageImage(BINDING_BLUR_SRC, VK_SHADER_STAGE_COMPUTE_BIT);
        descManager->appendStorageImage(BINDING_BLUR_DST, VK_SHADER_STAGE_COMPUTE_BIT);
        descManager->appendUBO(BINDING_BLUR_UBO, VK_SHADER_STAGE_COMPUTE_BIT, std::move(descUBO));
        appendUniqueDescriptor(SET_BLUR, descriptors, std::move(descManager));
    }
}
