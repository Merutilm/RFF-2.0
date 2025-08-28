//
// Created by Merutilm on 2025-08-28.
//

#include "BoxBlurPipelineConfigurator.hpp"

#include "../../vulkan_helper/util/ImageContextUtils.hpp"

namespace merutilm::rff2 {
    BoxBlurPipelineConfigurator::BoxBlurPipelineConfigurator(vkh::EngineRef engine) : ComputePipelineConfigurator(
        engine, "vk_box_blur.comp") {
    }

    void BoxBlurPipelineConfigurator::updateQueue(vkh::DescriptorUpdateQueue &queue, uint32_t frameIndex,
                                                  uint32_t imageIndex) {
        // no operation
    }

    void BoxBlurPipelineConfigurator::setImages(const uint32_t imageIndex, const vkh::ImageContext &srcImage,
                                                const vkh::ImageContext &dstImage) const {
        auto &desc = getDescriptor(SET_BLUR);
        auto &descManager = desc.getDescriptorManager();

        descManager.get<vkh::StorageImage>(BINDING_BLUR_SRC).ctx[imageIndex] = srcImage;
        descManager.get<vkh::StorageImage>(BINDING_BLUR_DST).ctx[imageIndex] = dstImage;


        writeDescriptorForEachFrame([&desc](vkh::DescriptorUpdateQueue &queue, const uint32_t inFlightIndex) {
            desc.queue(queue, inFlightIndex, inFlightIndex);
        });
    }


    void BoxBlurPipelineConfigurator::setBlurSize(const float blurSize) const {
        auto &desc = getDescriptor(SET_BLUR);
        auto &ubo = *desc.getDescriptorManager().get<vkh::Uniform>(BINDING_BLUR_UBO);
        ubo.getHostObject().set<float>(TARGET_BLUR_UBO_BLUR_SIZE, blurSize);

        writeDescriptorForEachFrame([&desc, &ubo](vkh::DescriptorUpdateQueue &queue, const uint32_t inFlightIndex) {
            ubo.update(inFlightIndex);
            desc.queue(queue, inFlightIndex, inFlightIndex);
        });
    }


    void BoxBlurPipelineConfigurator::configurePushConstant(vkh::PipelineLayoutManagerRef pipelineLayoutManager) {
        //no operation
    }

    void BoxBlurPipelineConfigurator::configureDescriptors(std::vector<vkh::DescriptorPtr> &descriptors) {
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
