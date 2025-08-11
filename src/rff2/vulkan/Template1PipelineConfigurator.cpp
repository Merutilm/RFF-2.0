//
// Created by Merutilm on 2025-07-29.
//

#include "../vulkan/Template1PipelineConfigurator.hpp"

#include "../../vulkan_helper/util/ImageContextUtils.hpp"

namespace merutilm::rff2 {
    Template1PipelineConfigurator::Template1PipelineConfigurator(const vkh::Engine &engine,
                                                                 const uint32_t
                                                                 subpassIndex) : GeneralPostProcessPipelineConfigurator(
        engine, subpassIndex, "vk_template-1.frag") {
        Template1PipelineConfigurator::configure();
    }


    void Template1PipelineConfigurator::updateQueue(vkh::DescriptorUpdateQueue &queue, const uint32_t frameIndex,
                                                    const uint32_t imageIndex,
                                                    const uint32_t width, const uint32_t height) {

        getDescriptor(0).queue(queue, frameIndex);
    }

    void Template1PipelineConfigurator::render(const VkCommandBuffer cbh, const uint32_t frameIndex,
                                               const uint32_t width, const uint32_t height) {
        draw(cbh, frameIndex, 0);
    }


    void Template1PipelineConfigurator::configurePushConstant(vkh::DescriptorSetLayoutRepo &layoutRepo,
                                                              vkh::PipelineLayoutManager &pipelineLayoutManager) {
        //noop
    }

    void Template1PipelineConfigurator::configureDescriptors(std::vector<const vkh::Descriptor *> &descriptors,
                                                             vkh::DescriptorSetLayoutRepo &layoutRepo,
                                                             vkh::SharedDescriptorRepo &descRepo) {
        auto texture = std::make_unique<vkh::Sampler2D>(engine.getCore(), VkSamplerCreateInfo{
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
                                                       .anisotropyEnable = VK_TRUE,
                                                       .maxAnisotropy = engine.getCore().getPhysicalDevice().
                                                       getPhysicalDeviceProperties().limits.maxSamplerAnisotropy,
                                                       .compareEnable = VK_TRUE,
                                                       .compareOp = VK_COMPARE_OP_LESS,
                                                       .minLod = 0,
                                                       .maxLod = 1,
                                                       .borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK,
                                                       .unnormalizedCoordinates = VK_FALSE
                                                   });
        texture->setImageContext(
            vkh::ImageContextUtils::imageFromPath(engine.getCore(), engine.getCommandPool(), "../res/icon.png"));

        auto manager = std::make_unique<vkh::DescriptorManager>();
        manager->appendCombinedImgSampler(0, VK_SHADER_STAGE_FRAGMENT_BIT, std::move(texture));
        appendUniqueDescriptor(0, descriptors, layoutRepo, std::move(manager));
    }
}
