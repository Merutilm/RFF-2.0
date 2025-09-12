//
// Created by Merutilm on 2025-07-19.
//

#pragma once
#include <memory>
#include <glm/glm.hpp>

#include "../../vulkan_helper/core/factory.hpp"
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

        void configure(const vkh::CoreRef core,
                                      std::vector<vkh::DescriptorManager> &managers) override {
            auto bufferManager = vkh::factory::create<vkh::HostDataObjectManager>();

            bufferManager->reserve<glm::mat4>(TARGET_CAMERA_MODEL);
            bufferManager->reserve<glm::mat4>(TARGET_CAMERA_VIEW);
            bufferManager->reserve<glm::mat4>(TARGET_CAMERA_PROJ);

            auto ubo = vkh::factory::create<vkh::Uniform>(core, std::move(bufferManager),
                                                          vkh::BufferLock::ALWAYS_MUTABLE, true);
            auto descManager = vkh::factory::create<vkh::DescriptorManager>();
            descManager->appendUBO(BINDING_UBO_CAMERA, STAGE, std::move(ubo));
            managers.emplace_back(std::move(descManager));
        }
    };

    struct DescTime final : public vkh::DescriptorTemplate {
        static constexpr uint32_t ID = 1;
        static constexpr VkShaderStageFlags STAGE = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT | VK_SHADER_STAGE_COMPUTE_BIT;

        static constexpr uint32_t BINDING_UBO_TIME = 0;

        static constexpr uint32_t TARGET_TIME_CURRENT = 0;

        void configure(const vkh::CoreRef core,
                                      std::vector<vkh::DescriptorManager> &managers) override {
            auto bufferManager = vkh::factory::create<vkh::HostDataObjectManager>();
            bufferManager->reserve<float>(TARGET_TIME_CURRENT);
            auto ubo = vkh::factory::create<vkh::Uniform>(core, std::move(bufferManager),
                                                          vkh::BufferLock::ALWAYS_MUTABLE, true);
            auto descManager = vkh::factory::create<vkh::DescriptorManager>();
            descManager->appendUBO(BINDING_UBO_TIME, STAGE, std::move(ubo));
            managers.emplace_back(std::move(descManager));
        }
    };

    struct DescIteration final : public vkh::DescriptorTemplate {
        static constexpr uint32_t ID = 2;
        static constexpr VkShaderStageFlags STAGE = VK_SHADER_STAGE_FRAGMENT_BIT | VK_SHADER_STAGE_COMPUTE_BIT;

        static constexpr uint32_t BINDING_UBO_ITERATION_INFO = 0;
        static constexpr uint32_t BINDING_SSBO_ITERATION_MATRIX = 1;

        static constexpr uint32_t TARGET_UBO_ITERATION_EXTENT = 0;
        static constexpr uint32_t TARGET_UBO_ITERATION_MAX = 1;

        static constexpr uint32_t TARGET_SSBO_ITERATION_BUFFER = 0;

        void configure(const vkh::CoreRef core,
                                      std::vector<vkh::DescriptorManager> &managers) override {
            auto descManager = vkh::factory::create<vkh::DescriptorManager>();

            auto infoManager = vkh::factory::create<vkh::HostDataObjectManager>();
            infoManager->reserve<glm::uvec2>(TARGET_UBO_ITERATION_EXTENT);
            infoManager->reserve<double>(TARGET_UBO_ITERATION_MAX);

            auto bufferManager = vkh::factory::create<vkh::HostDataObjectManager>();
            bufferManager->reserveArray<double>(TARGET_SSBO_ITERATION_BUFFER, 1);

            auto ubo = vkh::factory::create<vkh::Uniform>(core, std::move(infoManager),
                                                                vkh::BufferLock::LOCK_UNLOCK, false);
            auto ssbo = vkh::factory::create<vkh::ShaderStorage>(core, std::move(bufferManager),
                                                                vkh::BufferLock::LOCK_ONLY, false);
            descManager->appendUBO(BINDING_UBO_ITERATION_INFO, STAGE, std::move(ubo));
            descManager->appendSSBO(BINDING_SSBO_ITERATION_MATRIX, STAGE, std::move(ssbo));

            managers.emplace_back(std::move(descManager));
        }
    };


    struct DescPalette final : public vkh::DescriptorTemplate {
        static constexpr uint32_t ID = 3;
        static constexpr VkShaderStageFlags STAGE = VK_SHADER_STAGE_FRAGMENT_BIT | VK_SHADER_STAGE_COMPUTE_BIT;

        static constexpr uint32_t BINDING_SSBO_PALETTE = 0;

        static constexpr uint32_t TARGET_PALETTE_SIZE = 0;
        static constexpr uint32_t TARGET_PALETTE_INTERVAL = 1;
        static constexpr uint32_t TARGET_PALETTE_OFFSET = 2;
        static constexpr uint32_t TARGET_PALETTE_SMOOTHING = 3;
        static constexpr uint32_t TARGET_PALETTE_ANIMATION_SPEED = 4;
        static constexpr uint32_t TARGET_PALETTE_COLORS = 5;

        void configure(const vkh::CoreRef core,
                                      std::vector<vkh::DescriptorManager> &managers) override {
            auto bufferManager = vkh::factory::create<vkh::HostDataObjectManager>();
            bufferManager->reserve<uint32_t>(TARGET_PALETTE_SIZE);
            bufferManager->reserve<float>(TARGET_PALETTE_INTERVAL);
            bufferManager->reserve<double>(TARGET_PALETTE_OFFSET);
            bufferManager->reserve<uint32_t>(TARGET_PALETTE_SMOOTHING);
            bufferManager->reserve<float>(TARGET_PALETTE_ANIMATION_SPEED, 8);
            bufferManager->reserveArray<glm::vec4>(TARGET_PALETTE_COLORS, 0);

            auto ssbo = vkh::factory::create<vkh::ShaderStorage>(core, std::move(bufferManager),
                                                                 vkh::BufferLock::LOCK_UNLOCK, false);
            auto descManager = vkh::factory::create<vkh::DescriptorManager>();

            descManager->appendSSBO(BINDING_SSBO_PALETTE, STAGE, std::move(ssbo));
            managers.emplace_back(std::move(descManager));
        }
    };

    struct DescStripe final : public vkh::DescriptorTemplate {
        static constexpr uint32_t ID = 4;
        static constexpr VkShaderStageFlags STAGE = VK_SHADER_STAGE_COMPUTE_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;

        static constexpr uint32_t BINDING_UBO_STRIPE = 0;

        static constexpr uint32_t TARGET_STRIPE_TYPE = 0;
        static constexpr uint32_t TARGET_STRIPE_FIRST_INTERVAL = 1;
        static constexpr uint32_t TARGET_STRIPE_SECOND_INTERVAL = 2;
        static constexpr uint32_t TARGET_STRIPE_OPACITY = 3;
        static constexpr uint32_t TARGET_STRIPE_OFFSET = 4;
        static constexpr uint32_t TARGET_STRIPE_ANIMATION_SPEED = 5;


        void configure(const vkh::CoreRef core,
                                      std::vector<vkh::DescriptorManager> &managers) override {
            auto bufferManager = vkh::factory::create<vkh::HostDataObjectManager>();
            bufferManager->reserve<uint32_t>(TARGET_STRIPE_TYPE);
            bufferManager->reserve<float>(TARGET_STRIPE_FIRST_INTERVAL);
            bufferManager->reserve<float>(TARGET_STRIPE_SECOND_INTERVAL);
            bufferManager->reserve<float>(TARGET_STRIPE_OPACITY);
            bufferManager->reserve<float>(TARGET_STRIPE_OFFSET);
            bufferManager->reserve<float>(TARGET_STRIPE_ANIMATION_SPEED);
            auto ubo = vkh::factory::create<vkh::Uniform>(core, std::move(bufferManager), vkh::BufferLock::LOCK_UNLOCK, false);
            auto descManager = vkh::factory::create<vkh::DescriptorManager>();
            descManager->appendUBO(BINDING_UBO_STRIPE, STAGE, std::move(ubo));
            managers.emplace_back(std::move(descManager));
        }
    };

    struct DescSlope final : public vkh::DescriptorTemplate {
        static constexpr uint32_t ID = 5;
        static constexpr VkShaderStageFlags STAGE = VK_SHADER_STAGE_FRAGMENT_BIT;

        static constexpr uint32_t BINDING_UBO_SLOPE = 0;

        static constexpr uint32_t TARGET_SLOPE_DEPTH = 0;
        static constexpr uint32_t TARGET_SLOPE_REFLECTION_RATIO = 1;
        static constexpr uint32_t TARGET_SLOPE_OPACITY = 2;
        static constexpr uint32_t TARGET_SLOPE_ZENITH = 3;
        static constexpr uint32_t TARGET_SLOPE_AZIMUTH = 4;


        void configure(const vkh::CoreRef core,
                                      std::vector<vkh::DescriptorManager> &managers) override {
            auto bufferManager = vkh::factory::create<vkh::HostDataObjectManager>();
            bufferManager->reserve<float>(TARGET_SLOPE_DEPTH);
            bufferManager->reserve<float>(TARGET_SLOPE_REFLECTION_RATIO);
            bufferManager->reserve<float>(TARGET_SLOPE_OPACITY);
            bufferManager->reserve<float>(TARGET_SLOPE_ZENITH);
            bufferManager->reserve<float>(TARGET_SLOPE_AZIMUTH);
            auto ubo = vkh::factory::create<vkh::Uniform>(core, std::move(bufferManager), vkh::BufferLock::LOCK_UNLOCK, false);
            auto descManager = vkh::factory::create<vkh::DescriptorManager>();
            descManager->appendUBO(BINDING_UBO_SLOPE, STAGE, std::move(ubo));
            managers.emplace_back(std::move(descManager));
        }
    };

    struct DescColor final : public vkh::DescriptorTemplate {
        static constexpr uint32_t ID = 6;
        static constexpr VkShaderStageFlags STAGE = VK_SHADER_STAGE_FRAGMENT_BIT;

        static constexpr uint32_t BINDING_UBO_COLOR = 0;

        static constexpr uint32_t TARGET_COLOR_GAMMA = 0;
        static constexpr uint32_t TARGET_COLOR_EXPOSURE = 1;
        static constexpr uint32_t TARGET_COLOR_HUE = 2;
        static constexpr uint32_t TARGET_COLOR_SATURATION = 3;
        static constexpr uint32_t TARGET_COLOR_BRIGHTNESS = 4;
        static constexpr uint32_t TARGET_COLOR_CONTRAST = 5;


        void configure(const vkh::CoreRef core,
                                      std::vector<vkh::DescriptorManager> &managers) override {
            auto bufferManager = vkh::factory::create<vkh::HostDataObjectManager>();
            bufferManager->reserve<float>(TARGET_COLOR_GAMMA);
            bufferManager->reserve<float>(TARGET_COLOR_EXPOSURE);
            bufferManager->reserve<float>(TARGET_COLOR_HUE);
            bufferManager->reserve<float>(TARGET_COLOR_SATURATION);
            bufferManager->reserve<float>(TARGET_COLOR_BRIGHTNESS);
            bufferManager->reserve<float>(TARGET_COLOR_CONTRAST);
            auto ubo = vkh::factory::create<vkh::Uniform>(core, std::move(bufferManager), vkh::BufferLock::LOCK_UNLOCK, false);
            auto descManager = vkh::factory::create<vkh::DescriptorManager>();
            descManager->appendUBO(BINDING_UBO_COLOR, STAGE, std::move(ubo));
            managers.emplace_back(std::move(descManager));
        }
    };

    struct DescFog final : public vkh::DescriptorTemplate {
        static constexpr uint32_t ID = 7;
        static constexpr VkShaderStageFlags STAGE = VK_SHADER_STAGE_FRAGMENT_BIT;

        static constexpr uint32_t BINDING_UBO_FOG = 0;

        static constexpr uint32_t TARGET_FOG_RADIUS = 0;
        static constexpr uint32_t TARGET_FOG_OPACITY = 1;

        void configure(const vkh::CoreRef core,
                                      std::vector<vkh::DescriptorManager> &managers) override {
            auto descManager = vkh::factory::create<vkh::DescriptorManager>();

            auto bufferManager = vkh::factory::create<vkh::HostDataObjectManager>();
            bufferManager->reserve<float>(TARGET_FOG_RADIUS);
            bufferManager->reserve<float>(TARGET_FOG_OPACITY);
            auto ubo = vkh::factory::create<vkh::Uniform>(core, std::move(bufferManager), vkh::BufferLock::LOCK_UNLOCK, false);
            descManager->appendUBO(BINDING_UBO_FOG, STAGE, std::move(ubo));
            managers.emplace_back(std::move(descManager));
        }
    };

    struct DescBloom final : public vkh::DescriptorTemplate {
        static constexpr uint32_t ID = 8;
        static constexpr VkShaderStageFlags STAGE = VK_SHADER_STAGE_FRAGMENT_BIT;

        static constexpr uint32_t BINDING_UBO_BLOOM = 0;

        static constexpr uint32_t TARGET_BLOOM_THRESHOLD = 0;
        static constexpr uint32_t TARGET_BLOOM_RADIUS = 1;
        static constexpr uint32_t TARGET_BLOOM_SOFTNESS = 2;
        static constexpr uint32_t TARGET_BLOOM_INTENSITY = 3;


        void configure(const vkh::CoreRef core,
                                      std::vector<vkh::DescriptorManager> &managers) override {
            auto bufferManager = vkh::factory::create<vkh::HostDataObjectManager>();
            bufferManager->reserve<float>(TARGET_BLOOM_THRESHOLD);
            bufferManager->reserve<float>(TARGET_BLOOM_RADIUS);
            bufferManager->reserve<float>(TARGET_BLOOM_SOFTNESS);
            bufferManager->reserve<float>(TARGET_BLOOM_INTENSITY);
            auto ubo = vkh::factory::create<vkh::Uniform>(core, std::move(bufferManager), vkh::BufferLock::LOCK_UNLOCK, false);
            auto descManager = vkh::factory::create<vkh::DescriptorManager>();
            descManager->appendUBO(BINDING_UBO_BLOOM, STAGE, std::move(ubo));
            managers.emplace_back(std::move(descManager));
        }
    };

    struct DescLinearInterpolation final : public vkh::DescriptorTemplate {
        static constexpr uint32_t ID = 9;
        static constexpr VkShaderStageFlags STAGE = VK_SHADER_STAGE_FRAGMENT_BIT;

        static constexpr uint32_t BINDING_UBO_LINEAR_INTERPOLATION = 0;

        static constexpr uint32_t TARGET_LINEAR_INTERPOLATION_USE = 0;


        void configure(const vkh::CoreRef core,
                                      std::vector<vkh::DescriptorManager> &managers) override {
            auto bufferManager = vkh::factory::create<vkh::HostDataObjectManager>();
            bufferManager->reserve<bool>(TARGET_LINEAR_INTERPOLATION_USE, 3);
            auto ubo = vkh::factory::create<vkh::Uniform>(core, std::move(bufferManager), vkh::BufferLock::LOCK_UNLOCK, false);
            auto descManager = vkh::factory::create<vkh::DescriptorManager>();
            descManager->appendUBO(BINDING_UBO_LINEAR_INTERPOLATION, STAGE, std::move(ubo));
            managers.emplace_back(std::move(descManager));
        }
    };

    struct DescVideo final : public vkh::DescriptorTemplate {
        static constexpr uint32_t ID = 10;
        static constexpr VkShaderStageFlags STAGE = VK_SHADER_STAGE_FRAGMENT_BIT | VK_SHADER_STAGE_COMPUTE_BIT;

        static constexpr uint32_t BINDING_UBO_VIDEO = 0;

        static constexpr uint32_t TARGET_VIDEO_DEFAULT_ZOOM_INCREMENT = 0;
        static constexpr uint32_t TARGET_VIDEO_CURRENT_FRAME = 1;


        void configure(const vkh::CoreRef core,
                                      std::vector<vkh::DescriptorManager> &managers) override {
            auto bufferManager = vkh::factory::create<vkh::HostDataObjectManager>();
            bufferManager->reserve<float>(TARGET_VIDEO_DEFAULT_ZOOM_INCREMENT);
            bufferManager->reserve<float>(TARGET_VIDEO_CURRENT_FRAME);
            auto ubo = vkh::factory::create<vkh::Uniform>(core, std::move(bufferManager), vkh::BufferLock::LOCK_UNLOCK, true);
            auto descManager = vkh::factory::create<vkh::DescriptorManager>();
            descManager->appendUBO(BINDING_UBO_VIDEO, STAGE, std::move(ubo));
            managers.emplace_back(std::move(descManager));
        }
    };


}
