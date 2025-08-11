//
// Created by Merutilm on 2025-07-19.
//

#pragma once
#include <memory>
#include <glm/glm.hpp>

#include "../../vulkan_helper/manage/DescriptorManager.hpp"
#include "../../vulkan_helper/struct/DescriptorTemplate.hpp"

namespace merutilm::rff2::SharedDescriptorTemplate {

    struct DescCamera3D final : public vkh::DescriptorTemplate{

        static constexpr uint32_t ID = 0;
        static constexpr VkShaderStageFlags STAGE = VK_SHADER_STAGE_VERTEX_BIT;

        static constexpr uint32_t BINDING_UBO_CAMERA = 0;

        static constexpr uint32_t TARGET_CAMERA_MODEL = 0;
        static constexpr uint32_t TARGET_CAMERA_VIEW = 1;
        static constexpr uint32_t TARGET_CAMERA_PROJ = 2;

        std::unique_ptr<vkh::DescriptorManager> create(const vkh::Core &core) override {
            auto bufferManager = std::make_unique<vkh::ShaderObjectManager>();

            bufferManager->reserve<glm::mat4>(TARGET_CAMERA_MODEL);
            bufferManager->reserve<glm::mat4>(TARGET_CAMERA_VIEW);
            bufferManager->reserve<glm::mat4>(TARGET_CAMERA_PROJ);

            auto ubo = std::make_unique<vkh::Uniform>(core, std::move(bufferManager));
            auto descManager = std::make_unique<vkh::DescriptorManager>();
            descManager->appendUBO(BINDING_UBO_CAMERA, STAGE, std::move(ubo));
            return descManager;
        }
    };

    struct DescTime final : public vkh::DescriptorTemplate{

        static constexpr uint32_t ID = 1;
        static constexpr VkShaderStageFlags STAGE = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;

        static constexpr uint32_t BINDING_UBO_TIME = 0;

        static constexpr uint32_t TARGET_TIME_CURRENT = 0;

        std::unique_ptr<vkh::DescriptorManager> create(const vkh::Core &core) override {
            auto bufferManager = std::make_unique<vkh::ShaderObjectManager>();
            bufferManager->reserve<float>(TARGET_TIME_CURRENT);
            auto ubo = std::make_unique<vkh::Uniform>(core, std::move(bufferManager));
            auto descManager = std::make_unique<vkh::DescriptorManager>();
            descManager->appendUBO(BINDING_UBO_TIME, STAGE, std::move(ubo));
            return descManager;
        }
    };

    struct DescIteration final : public vkh::DescriptorTemplate {
        static constexpr uint32_t ID = 2;
        static constexpr VkShaderStageFlags STAGE = VK_SHADER_STAGE_FRAGMENT_BIT;

        static constexpr uint32_t BINDING_SAMPLER_ITERATION = 0;
        static constexpr uint32_t BINDING_UBO_ITERATION = 1;

        static constexpr uint32_t TARGET_ITERATION_MAX = 0;

        std::unique_ptr<vkh::DescriptorManager> create(const vkh::Core &core) override {
            auto bufferManager = std::make_unique<vkh::ShaderObjectManager>();
            bufferManager->reserve<double>(TARGET_ITERATION_MAX);
            auto ubo = std::make_unique<vkh::Uniform>(core, std::move(bufferManager));
            auto descManager = std::make_unique<vkh::DescriptorManager>();
            descManager->appendCombinedImgSampler(BINDING_SAMPLER_ITERATION, STAGE, std::make_unique<vkh::Sampler2D>(core, VkSamplerCreateInfo{
                .sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,
                .pNext = nullptr,
                .flags = 0,
                .magFilter = VK_FILTER_NEAREST,
                .minFilter = VK_FILTER_NEAREST,
                .mipmapMode = VK_SAMPLER_MIPMAP_MODE_NEAREST,
                .addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER,
                .addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER,
                .addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER,
                .mipLodBias = 0,
                .anisotropyEnable = VK_FALSE,
                .maxAnisotropy = 0,
                .compareEnable = VK_FALSE,
                .compareOp = VK_COMPARE_OP_ALWAYS,
                .minLod = 0,
                .maxLod = 0,
                .borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK,
                .unnormalizedCoordinates = VK_TRUE
            }));
            descManager->appendUBO(BINDING_UBO_ITERATION, STAGE, std::move(ubo));
            return descManager;
        }
    };


    struct DescPalette final : public vkh::DescriptorTemplate {
        static constexpr uint32_t ID = 3;
        static constexpr VkShaderStageFlags STAGE = VK_SHADER_STAGE_FRAGMENT_BIT;

        static constexpr uint32_t BINDING_SAMPLER_PALETTE = 0;
        static constexpr uint32_t BINDING_UBO_PALETTE = 1;

        static constexpr uint32_t TARGET_PALETTE_WIDTH = 0;
        static constexpr uint32_t TARGET_PALETTE_HEIGHT = 1;
        static constexpr uint32_t TARGET_PALETTE_SIZE = 2;
        static constexpr uint32_t TARGET_PALETTE_INTERVAL = 3;
        static constexpr uint32_t TARGET_PALETTE_OFFSET = 4;
        static constexpr uint32_t TARGET_PALETTE_SMOOTHING = 5;
        std::unique_ptr<vkh::DescriptorManager> create(const vkh::Core &core) override {
            auto bufferManager = std::make_unique<vkh::ShaderObjectManager>();
            bufferManager->reserve<int>(TARGET_PALETTE_WIDTH);
            bufferManager->reserve<int>(TARGET_PALETTE_HEIGHT);
            bufferManager->reserve<int>(TARGET_PALETTE_SIZE);
            bufferManager->reserve<float>(TARGET_PALETTE_INTERVAL);
            bufferManager->reserve<double>(TARGET_PALETTE_OFFSET);
            bufferManager->reserve<int>(TARGET_PALETTE_SMOOTHING);

            auto ubo = std::make_unique<vkh::Uniform>(core, std::move(bufferManager));
            auto descManager = std::make_unique<vkh::DescriptorManager>();

            descManager->appendCombinedImgSampler(BINDING_SAMPLER_PALETTE, STAGE, std::make_unique<vkh::Sampler2D>(core, VkSamplerCreateInfo{
                .sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,
                .pNext = nullptr,
                .flags = 0,
                .magFilter = VK_FILTER_LINEAR,
                .minFilter = VK_FILTER_LINEAR,
                .mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR,
                .addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER,
                .addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER,
                .addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER,
                .mipLodBias = 0,
                .anisotropyEnable = VK_FALSE,
                .maxAnisotropy = 0,
                .compareEnable = VK_FALSE,
                .compareOp = VK_COMPARE_OP_ALWAYS,
                .minLod = 0,
                .maxLod = 0,
                .borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK,
                .unnormalizedCoordinates = VK_FALSE,
            }));
            descManager->appendUBO(BINDING_UBO_PALETTE, STAGE, std::move(ubo));
            return descManager;
        }
    };

}
