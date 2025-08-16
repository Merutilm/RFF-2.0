//
// Created by Merutilm on 2025-07-19.
//

#pragma once
#include <memory>
#include <glm/glm.hpp>

#include "../../vulkan_helper/def/Factory.hpp"
#include "../../vulkan_helper/manage/DescriptorManager.hpp"
#include "../../vulkan_helper/struct/DescriptorTemplate.hpp"

namespace merutilm::rff2::SharedDescriptorTemplate {
    struct DescCamera3D final : public vkh::DescriptorTemplate {
        static constexpr uint32_t ID = 0;
        static constexpr VkShaderStageFlags STAGE = VK_SHADER_STAGE_VERTEX_BIT;

        static constexpr uint32_t BINDING_UBO_CAMERA = 0;

        static constexpr uint32_t TARGET_CAMERA_MODEL = 0;
        static constexpr uint32_t TARGET_CAMERA_VIEW = 1;
        static constexpr uint32_t TARGET_CAMERA_PROJ = 2;

        vkh::DescriptorManager create(const vkh::Core &core,
                                                       const vkh::DescriptorRequiresRepoContext &context) override {
            auto bufferManager = vkh::Factory::create<vkh::HostBufferObjectManager>();

            bufferManager->reserve<glm::mat4>(TARGET_CAMERA_MODEL);
            bufferManager->reserve<glm::mat4>(TARGET_CAMERA_VIEW);
            bufferManager->reserve<glm::mat4>(TARGET_CAMERA_PROJ);

            auto ubo = std::make_unique<vkh::Uniform>(core, std::move(bufferManager), vkh::BufferLock::ALWAYS_MUTABLE);
            auto descManager = vkh::Factory::create<vkh::DescriptorManager>();
            descManager->appendUBO(BINDING_UBO_CAMERA, STAGE, std::move(ubo));
            return descManager;
        }
    };

    struct DescTime final : public vkh::DescriptorTemplate {
        static constexpr uint32_t ID = 1;
        static constexpr VkShaderStageFlags STAGE = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;

        static constexpr uint32_t BINDING_UBO_TIME = 0;

        static constexpr uint32_t TARGET_TIME_CURRENT = 0;

        vkh::DescriptorManager create(const vkh::Core &core,
                                                       const vkh::DescriptorRequiresRepoContext &context) override {
            auto bufferManager = vkh::Factory::create<vkh::HostBufferObjectManager>();
            bufferManager->reserve<float>(TARGET_TIME_CURRENT);
            auto ubo = std::make_unique<vkh::Uniform>(core, std::move(bufferManager), vkh::BufferLock::ALWAYS_MUTABLE);
            auto descManager = vkh::Factory::create<vkh::DescriptorManager>();
            descManager->appendUBO(BINDING_UBO_TIME, STAGE, std::move(ubo));
            return descManager;
        }
    };

    struct DescIteration final : public vkh::DescriptorTemplate {
        static constexpr uint32_t ID = 2;
        static constexpr VkShaderStageFlags STAGE = VK_SHADER_STAGE_FRAGMENT_BIT;

        static constexpr uint32_t BINDING_SSBO_ITERATION = 0;

        static constexpr uint32_t TARGET_ITERATION_EXTENT = 0;
        static constexpr uint32_t TARGET_ITERATION_MAX = 1;
        static constexpr uint32_t TARGET_ITERATION_BUFFER = 2;

        vkh::DescriptorManager create(const vkh::Core &core,
                                                       const vkh::DescriptorRequiresRepoContext &context) override {
            auto bufferManager = vkh::Factory::create<vkh::HostBufferObjectManager>();
            bufferManager->reserve<glm::uvec2>(TARGET_ITERATION_EXTENT);
            bufferManager->reserve<double>(TARGET_ITERATION_MAX);
            bufferManager->reserveArray<double>(TARGET_ITERATION_BUFFER, 0);
            auto ssbo = std::make_unique<vkh::ShaderStorage>(core, std::move(bufferManager),
                                                             vkh::BufferLock::LOCK_UNLOCK);
            auto descManager = vkh::Factory::create<vkh::DescriptorManager>();
            descManager->appendSSBO(BINDING_SSBO_ITERATION, STAGE, std::move(ssbo));
            return descManager;
        }
    };


    struct DescPalette final : public vkh::DescriptorTemplate {
        static constexpr uint32_t ID = 3;
        static constexpr VkShaderStageFlags STAGE = VK_SHADER_STAGE_FRAGMENT_BIT;

        static constexpr uint32_t BINDING_SAMPLER_PALETTE = 0;
        static constexpr uint32_t BINDING_UBO_PALETTE = 1;

        static constexpr uint32_t TARGET_PALETTE_EXTENT = 0;
        static constexpr uint32_t TARGET_PALETTE_SIZE = 1;
        static constexpr uint32_t TARGET_PALETTE_INTERVAL = 2;
        static constexpr uint32_t TARGET_PALETTE_OFFSET = 3;
        static constexpr uint32_t TARGET_PALETTE_SMOOTHING = 4;
        static constexpr uint32_t TARGET_PALETTE_ANIMATION_SPEED = 5;

        vkh::DescriptorManager create(const vkh::Core &core,
                                                       const vkh::DescriptorRequiresRepoContext &context) override {
            auto bufferManager = vkh::Factory::create<vkh::HostBufferObjectManager>();
            bufferManager->reserve<glm::uvec2>(TARGET_PALETTE_EXTENT);
            bufferManager->reserve<uint32_t>(TARGET_PALETTE_SIZE);
            bufferManager->reserve<float>(TARGET_PALETTE_INTERVAL);
            bufferManager->reserve<double>(TARGET_PALETTE_OFFSET);
            bufferManager->reserve<uint32_t>(TARGET_PALETTE_SMOOTHING);
            bufferManager->reserve<float>(TARGET_PALETTE_ANIMATION_SPEED);

            auto ubo = std::make_unique<vkh::Uniform>(core, std::move(bufferManager), vkh::BufferLock::LOCK_UNLOCK);
            auto descManager = vkh::Factory::create<vkh::DescriptorManager>();

            const vkh::Sampler &sampler = context.samplerRepo.pick(VkSamplerCreateInfo{
                .sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,
                .pNext = nullptr,
                .flags = 0,
                .magFilter = VK_FILTER_LINEAR,
                .minFilter = VK_FILTER_LINEAR,
                .mipmapMode = VK_SAMPLER_MIPMAP_MODE_NEAREST,
                .addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
                .addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
                .addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
                .mipLodBias = 0,
                .anisotropyEnable = VK_FALSE,
                .maxAnisotropy = 0,
                .compareEnable = VK_FALSE,
                .compareOp = VK_COMPARE_OP_ALWAYS,
                .minLod = 0,
                .maxLod = 0,
                .borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK,
                .unnormalizedCoordinates = VK_FALSE,
            });
            descManager->appendCombinedImgSampler(BINDING_SAMPLER_PALETTE, STAGE,
                                                  std::make_unique<vkh::CombinedImageSampler>(core, sampler));
            descManager->appendUBO(BINDING_UBO_PALETTE, STAGE, std::move(ubo));
            return descManager;
        }
    };

    struct DescStripe final : public vkh::DescriptorTemplate {
        static constexpr uint32_t ID = 4;
        static constexpr VkShaderStageFlags STAGE = VK_SHADER_STAGE_FRAGMENT_BIT;

        static constexpr uint32_t BINDING_UBO_STRIPE = 0;

        static constexpr uint32_t TARGET_STRIPE_TYPE = 0;
        static constexpr uint32_t TARGET_STRIPE_FIRST_INTERVAL = 1;
        static constexpr uint32_t TARGET_STRIPE_SECOND_INTERVAL = 2;
        static constexpr uint32_t TARGET_STRIPE_OPACITY = 3;
        static constexpr uint32_t TARGET_STRIPE_OFFSET = 4;
        static constexpr uint32_t TARGET_STRIPE_ANIMATION_SPEED = 5;


        vkh::DescriptorManager create(const vkh::Core &core,
                                                       const vkh::DescriptorRequiresRepoContext &context) override {
            auto bufferManager = vkh::Factory::create<vkh::HostBufferObjectManager>();
            bufferManager->reserve<uint32_t>(TARGET_STRIPE_TYPE);
            bufferManager->reserve<float>(TARGET_STRIPE_FIRST_INTERVAL);
            bufferManager->reserve<float>(TARGET_STRIPE_SECOND_INTERVAL);
            bufferManager->reserve<float>(TARGET_STRIPE_OPACITY);
            bufferManager->reserve<float>(TARGET_STRIPE_OFFSET);
            bufferManager->reserve<float>(TARGET_STRIPE_ANIMATION_SPEED);
            auto ubo = std::make_unique<vkh::Uniform>(core, std::move(bufferManager), vkh::BufferLock::LOCK_UNLOCK);
            auto descManager = vkh::Factory::create<vkh::DescriptorManager>();
            descManager->appendUBO(BINDING_UBO_STRIPE, STAGE, std::move(ubo));
            return descManager;
        }
    };
}
