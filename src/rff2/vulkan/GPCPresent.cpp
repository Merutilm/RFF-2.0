//
// Created by Merutilm on 2025-08-27.
//

#include "GPCPresent.hpp"

#include "RCC4.hpp"
#include "SharedDescriptorTemplate.hpp"

namespace merutilm::rff2 {
    void GPCPresent::updateQueue(vkh::DescriptorUpdateQueue &queue, const uint32_t frameIndex) {
        //no operation
    }

    void GPCPresent::pipelineInitialized() {
        //no operation
    }

    void GPCPresent::windowResized() {
        const auto &context = engine.getSharedImageContext().getMultiframeContext(SharedImageContextIndices::MF_RENDER_IMAGE_PRIMARY);
        auto &resultDesc = getDescriptor(SET_RESULT);
        resultDesc.get<vkh::CombinedMultiframeImageSampler>(0, BINDING_RESULT_SAMPLER)->setImageContext(context);

        writeDescriptorForEachFrame(
            [&resultDesc](vkh::DescriptorUpdateQueue &queue, const uint32_t frameIndex) {
                resultDesc.queue(queue, frameIndex, {}, std::vector{BINDING_RESULT_SAMPLER});
            });
    }

    void GPCPresent::configurePushConstant(vkh::PipelineLayoutManagerRef pipelineLayoutManager) {
        //noop
    }


    void GPCPresent::configureDescriptors(std::vector<vkh::DescriptorPtr> &descriptors) {
        using namespace SharedDescriptorTemplate;
        auto descManager = vkh::factory::create<vkh::DescriptorManager>();
        vkh::SamplerRef sampler = pickFromRepository<vkh::SamplerRepo, vkh::SamplerRef>(
            VkSamplerCreateInfo{
                .sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,
                .pNext = nullptr,
                .flags = 0,
                .magFilter = VK_FILTER_LINEAR,
                .minFilter = VK_FILTER_LINEAR,
                .mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR,
                .addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT,
                .addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT,
                .addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT,
                .mipLodBias = 0,
                .anisotropyEnable = VK_FALSE,
                .maxAnisotropy = 0,
                .compareEnable = VK_FALSE,
                .compareOp = VK_COMPARE_OP_ALWAYS,
                .minLod = 0,
                .maxLod = 0,
                .borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK,
                .unnormalizedCoordinates = VK_FALSE
            });
        auto combinedSampler = vkh::factory::create<vkh::CombinedMultiframeImageSampler>(engine.getCore(), sampler);
        descManager->appendCombinedMultiframeImgSampler(BINDING_RESULT_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT,
                                                        std::move(combinedSampler));
        appendUniqueDescriptor(SET_RESULT, descriptors, std::move(descManager));
        appendDescriptor<DescResolution>(SET_RESOLUTION, descriptors);
    }
}
