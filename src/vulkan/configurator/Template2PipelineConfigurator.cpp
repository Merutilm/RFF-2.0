//
// Created by Merutilm on 2025-07-20.
//

#include "Template2PipelineConfigurator.hpp"

#include "BasicRenderContextConfigurator.hpp"
#include "../repo/SharedDescriptorRepo.hpp"
#include "../struct/Vertex.hpp"

namespace merutilm::mvk {
    void Template2PipelineConfigurator::updateQueue(DescriptorUpdateQueue &queue, const uint32_t frameIndex,
                                                    const uint32_t imageIndex,
                                                    const uint32_t width, const uint32_t height) {

        updateUninitializedVertexIndex();

        //TIME
        auto &descTime = getDescriptor(SET_TIME);
        const auto &descTimeCurrent = descTime.getDescriptorManager().get<std::unique_ptr<Uniform> >(
            DSSharedTemplate::Time::UBO_TIME);
        descTimeCurrent->set(DSSharedTemplate::Time::TIME_CURRENT, engine.getCore().getTime());
        descTimeCurrent->update(frameIndex);
        descTime.updateQueue(queue, frameIndex);

        //INPUT
        auto &descInput = getDescriptor(SET_INPUT);
        descInput.getDescriptorManager().get<std::unique_ptr<
            Sampler2D> >(BINDING_INPUT_SAMPLER)->setImageContext(
            getRenderContextConfigurator<BasicRenderContextConfigurator>().getImageContext(
                BasicRenderContextConfigurator::PRIMARY_SUBPASS_RESULT_IMAGE)[imageIndex]);
        descInput.getDescriptorManager().get<ImageContext>(BINDING_INPUT_ATTACHMENT_ELEM) =
                getRenderContextConfigurator<BasicRenderContextConfigurator>().getImageContext(
                    BasicRenderContextConfigurator::PRIMARY_SUBPASS_RESULT_IMAGE)[imageIndex];
        descInput.updateQueue(queue, frameIndex);

        getPushConstantManager(PUSH_RESOLUTION).set(PUSH_RESOLUTION_SIZE, glm::vec2{
                                                        static_cast<float>(width), static_cast<float>(height)
                                                    });
    }

    void Template2PipelineConfigurator::render(const VkCommandBuffer cbh, const uint32_t frameIndex,
                                               const uint32_t width,
                                               const uint32_t height) {
        pushAll(cbh);
        draw(cbh, frameIndex, 0);
    }


    void Template2PipelineConfigurator::configureDescriptors(std::vector<const Descriptor *> &descriptors,
                                                             DescriptorSetLayoutRepo &layoutRepo,
                                                             SharedDescriptorRepo &repo) {
        appendDescriptor(SET_TIME, descriptors, layoutRepo, repo, DescriptorName::TIME, VK_SHADER_STAGE_FRAGMENT_BIT);

        auto inputManager = std::make_unique<DescriptorManager>();
        inputManager->appendCombinedImgSampler(BINDING_INPUT_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT, std::make_unique<Sampler2D>(engine.getCore(), VkSamplerCreateInfo{
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
            .compareEnable = VK_FALSE,
            .compareOp = VK_COMPARE_OP_ALWAYS,
            .minLod = 0,
            .maxLod = 1,
            .borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK,
            .unnormalizedCoordinates = VK_FALSE
        }));
        inputManager->appendInputAttachment(BINDING_INPUT_ATTACHMENT_ELEM, VK_SHADER_STAGE_FRAGMENT_BIT);

        appendUniqueDescriptor(SET_INPUT, descriptors, layoutRepo, std::move(inputManager));
    }

    void Template2PipelineConfigurator::configurePushConstant(DescriptorSetLayoutRepo &layoutRepo,
                                                              PipelineLayoutManager &pipelineLayoutManager) {
        pipelineLayoutManager.appendPushConstantManager(PUSH_RESOLUTION, VK_SHADER_STAGE_FRAGMENT_BIT,
                                                        PushConstantReserve<glm::vec2>{PUSH_RESOLUTION_SIZE}
        );
    }

}
