//
// Created by Merutilm on 2025-07-20.
//

#include "../vulkan/Template2PipelineConfigurator.hpp"

#include "RFFRenderContextConfigurator.hpp"
#include "../../vulkan_helper/repo/SharedDescriptorRepo.hpp"
#include "../../vulkan_helper/struct/Vertex.hpp"
#include "SharedDescriptorTemplate.hpp"
#include "../../vulkan_helper/util/ImageContextUtils.hpp"

namespace merutilm::rff2 {
    Template2PipelineConfigurator::Template2PipelineConfigurator(const vkh::Engine &engine, const uint32_t subpassIndex)
        : GeneralPostProcessPipelineConfigurator(engine, subpassIndex, "vk_template-2.frag") {
        Template2PipelineConfigurator::init();
    }

    Template2PipelineConfigurator::~Template2PipelineConfigurator() {
        Template2PipelineConfigurator::destroy();
    }


    void Template2PipelineConfigurator::updateQueue(vkh::DescriptorUpdateQueue &queue, const uint32_t frameIndex,
                                                    const uint32_t imageIndex,
                                                    const uint32_t width, const uint32_t height) {
        //TIME
        auto &descTime = getDescriptor(SET_TIME);
        const auto &descTimeCurrent = descTime.getDescriptorManager().get<std::unique_ptr<vkh::Uniform> >(
            SharedDescriptorTemplate::DescTime::BINDING_UBO_TIME);
        descTimeCurrent->getHostObject().set(SharedDescriptorTemplate::DescTime::TARGET_TIME_CURRENT, engine.getCore().getTime());
        descTimeCurrent->update(frameIndex);
        descTime.queue(queue, frameIndex);

        //INPUT
        auto &descInput = getDescriptor(SET_INPUT);

        descInput.getDescriptorManager().get<vkh::ImageContext>(BINDING_INPUT_ATTACHMENT_ELEM) =
                getRenderContextConfigurator<RFFRenderContextConfigurator>().getImageContext(
                    RFFRenderContextConfigurator::PRIMARY_SUBPASS_RESULT_COLOR_IMAGE)[imageIndex];
        descInput.queue(queue, frameIndex);

        getPushConstantManager(PUSH_RESOLUTION).set(PUSH_RESOLUTION_SIZE, glm::vec2{
                                                        static_cast<float>(width), static_cast<float>(height)
                                                    });
    }


    void Template2PipelineConfigurator::configureDescriptors(std::vector<const vkh::Descriptor *> &descriptors) {
        appendDescriptor<SharedDescriptorTemplate::DescTime>(SET_TIME, descriptors);
        VkSamplerCreateInfo samplerInfo = {
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
            .maxAnisotropy = engine.getCore().getPhysicalDevice().getPhysicalDeviceProperties().limits.
            maxSamplerAnisotropy,
            .compareEnable = VK_FALSE,
            .compareOp = VK_COMPARE_OP_ALWAYS,
            .minLod = 0,
            .maxLod = 1,
            .borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK,
            .unnormalizedCoordinates = VK_FALSE
        };

        auto inputManager = std::make_unique<vkh::DescriptorManager>();
        auto sampler = std::make_unique<vkh::CombinedImageSampler>(
                                                   engine.getCore(),
                                                   engine.getRepositories().getRepository<vkh::SamplerRepo>()->pick(
                                                       std::move(samplerInfo)));
        auto context = vkh::ImageContextUtils::imageFromPath(engine.getCore(), engine.getCommandPool(), "../res/icon.png");

        sampler->setUniqueImageContext(std::move(context));

        inputManager->appendCombinedImgSampler(BINDING_INPUT_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT, std::move(sampler));
        inputManager->appendInputAttachment(BINDING_INPUT_ATTACHMENT_ELEM, VK_SHADER_STAGE_FRAGMENT_BIT);
        appendUniqueDescriptor(SET_INPUT, descriptors, std::move(inputManager));
    }

    void Template2PipelineConfigurator::configurePushConstant(vkh::PipelineLayoutManager &pipelineLayoutManager) {
        pipelineLayoutManager.appendPushConstantManager(PUSH_RESOLUTION, VK_SHADER_STAGE_FRAGMENT_BIT,
                                                        vkh::PushConstantReserve<glm::vec2>{PUSH_RESOLUTION_SIZE}
        );
    }

    void Template2PipelineConfigurator::init() {
        //noop
    }

    void Template2PipelineConfigurator::destroy() {
        //noop
    }


}
