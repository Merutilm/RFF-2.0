//
// Created by Merutilm on 2025-07-29.
//

#include "Template1PipelineConfigurator.hpp"

#include "../struct/Vertex.hpp"
#include "../util/ImageContextUtils.hpp"

namespace merutilm::mvk {




    void Template1PipelineConfigurator::updateQueue(DescriptorUpdateQueue &queue, const uint32_t frameIndex, const uint32_t imageIndex,
                                               const uint32_t width, const uint32_t height) {
        getVertexBuffer().update(frameIndex);
        getIndexBuffer().update(frameIndex);
        getDescriptor(0).updateQueue(queue, frameIndex);
    }

    void Template1PipelineConfigurator::render(const VkCommandBuffer cbh, const uint32_t frameIndex, const uint32_t width, const uint32_t height) {
        draw(cbh, frameIndex, 0);
    }


    void Template1PipelineConfigurator::configurePushConstant(DescriptorSetLayoutRepo &layoutRepo,
        PipelineLayoutManager &pipelineLayoutManager) {
        //noop
    }

    void Template1PipelineConfigurator::configureDescriptors(std::vector<const Descriptor *> &descriptors,
    DescriptorSetLayoutRepo &layoutRepo, SharedDescriptorRepo &descRepo) {

        auto texture = std::make_unique<Sampler2D>(engine.getCore(), VkSamplerCreateInfo{
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
            .maxAnisotropy = engine.getCore().getPhysicalDevice().getPhysicalDeviceProperties().limits.maxSamplerAnisotropy,
            .compareEnable = VK_TRUE,
            .compareOp = VK_COMPARE_OP_LESS,
            .minLod = 0,
            .maxLod = 1,
            .borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK,
            .unnormalizedCoordinates = VK_FALSE
        });
        texture->setImageContext(ImageContextUtils::imageFromPath(engine.getCore(), engine.getCommandPool(), "../res/icon.png"));

        auto manager = std::make_unique<DescriptorManager>();
        manager->appendCombinedImgSampler(0, VK_SHADER_STAGE_FRAGMENT_BIT, std::move(texture));
        appendUniqueDescriptor(0, descriptors, layoutRepo, std::move(manager));
    }

    void Template1PipelineConfigurator::configureVertexBuffer(ShaderObjectManager &som) {
        som.add(0, std::vector{
                    Vertex::generate({1, 1, 0}, {1, 1, 1}, {1, 1}),
                    Vertex::generate({1, -1, 0}, {1, 1, 1}, {1, 0}),
                    Vertex::generate({-1, -1, 0}, {1, 1, 1}, {0, 0}),
                    Vertex::generate({-1, 1, 0}, {1, 1, 1}, {0, 1}),
                });
    }

    void Template1PipelineConfigurator::configureIndexBuffer(ShaderObjectManager &som) {
        som.add(0, std::vector<uint32_t>{0, 1, 2, 2, 3, 0});
    }
}
