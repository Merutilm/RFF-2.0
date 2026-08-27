//
// Created by Merutilm on 2025-07-19.
//

#pragma once
#include <glm/glm.hpp>
#include <memory>
#include "../mrthy/PA.h"
#include "../mrthy/MPAIndexMapper.hpp"
#include "../data/ComputeShaderBatchStagingData.hpp"
#include "vulkan_helper/engine/manage/DescriptorManager.hpp"
#include "vulkan_helper/engine/wrapped/DescriptorTemplate.hpp"

namespace merutilm::rff2::SharedDescriptorTemplate {
    struct DescCamera3D final : public vkh::DescriptorTemplate {
        static constexpr uint32_t ID = 0;
        static constexpr VkShaderStageFlags STAGE = VK_SHADER_STAGE_VERTEX_BIT;

        static constexpr uint32_t BINDING_UBO_CAMERA = 0;

        static constexpr uint32_t TARGET_CAMERA_MODEL = 0;
        static constexpr uint32_t TARGET_CAMERA_VIEW = 1;
        static constexpr uint32_t TARGET_CAMERA_PROJ = 2;

        void configure(vkh::Core &core, std::vector<vkh::DescriptorManager> &managers) override {
            auto bufferManager = vkh::HostDataObjectManager();

            bufferManager.reserve<glm::mat4>(TARGET_CAMERA_MODEL);
            bufferManager.reserve<glm::mat4>(TARGET_CAMERA_VIEW);
            bufferManager.reserve<glm::mat4>(TARGET_CAMERA_PROJ);

            auto ubo = std::make_unique<vkh::Uniform>(core, std::move(bufferManager),
                                                      vkh::BufferLocalization::ALWAYS_EXPOSED, true);
            auto descManager = vkh::DescriptorManager();
            descManager.appendUBO(BINDING_UBO_CAMERA, STAGE, std::move(ubo));
            managers.emplace_back(std::move(descManager));
        }
    };

    struct DescTime final : public vkh::DescriptorTemplate {
        static constexpr uint32_t ID = 1;
        static constexpr VkShaderStageFlags STAGE =
                VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT | VK_SHADER_STAGE_COMPUTE_BIT;

        static constexpr uint32_t BINDING_UBO_TIME = 0;

        static constexpr uint32_t TARGET_TIME_CURRENT = 0;

        void configure(vkh::Core &core, std::vector<vkh::DescriptorManager> &managers) override {
            auto bufferManager = vkh::HostDataObjectManager();
            bufferManager.reserve<float>(TARGET_TIME_CURRENT);
            auto ubo = std::make_unique<vkh::Uniform>(core, std::move(bufferManager),
                                                      vkh::BufferLocalization::ALWAYS_EXPOSED, true);
            auto descManager = vkh::DescriptorManager();
            descManager.appendUBO(BINDING_UBO_TIME, STAGE, std::move(ubo));
            managers.emplace_back(std::move(descManager));
        }
    };

    struct DescIteration final : public vkh::DescriptorTemplate {
        static constexpr uint32_t ID = 2;
        static constexpr VkShaderStageFlags STAGE =
                VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT | VK_SHADER_STAGE_COMPUTE_BIT;

        static constexpr uint32_t BINDING_UBO_ITERATION_INFO = 0;
        static constexpr uint32_t BINDING_SSBO_ITERATION_MATRIX = 1;

        static constexpr uint32_t TARGET_UBO_ITERATION_EXTENT = 0;
        static constexpr uint32_t TARGET_UBO_ITERATION_MAX = 1;

        static constexpr uint32_t TARGET_SSBO_ITERATION_BUFFER = 0;

        void configure(vkh::Core &core, std::vector<vkh::DescriptorManager> &managers) override {
            auto descManager = vkh::DescriptorManager();

            auto infoManager = vkh::HostDataObjectManager();
            infoManager.reserve<glm::uvec2>(TARGET_UBO_ITERATION_EXTENT);
            infoManager.reserve<double>(TARGET_UBO_ITERATION_MAX);

            auto bufferManager = vkh::HostDataObjectManager();
            bufferManager.reserveArray<double>(TARGET_SSBO_ITERATION_BUFFER, 1);

            auto ubo = std::make_unique<vkh::Uniform>(core, std::move(infoManager),
                                                      vkh::BufferLocalization::BIDIRECTIONAL, false);
            auto ssbo = std::make_unique<vkh::ShaderStorage>(core, std::move(bufferManager),
                                                             vkh::BufferLocalization::BIDIRECTIONAL, false);
            descManager.appendUBO(BINDING_UBO_ITERATION_INFO, STAGE, std::move(ubo));
            descManager.appendSSBO(BINDING_SSBO_ITERATION_MATRIX, STAGE, std::move(ssbo));

            managers.emplace_back(std::move(descManager));
        }
    };


    struct DescPalette final : public vkh::DescriptorTemplate {
        static constexpr uint32_t ID = 3;
        static constexpr VkShaderStageFlags STAGE =
                VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT | VK_SHADER_STAGE_COMPUTE_BIT;

        static constexpr uint32_t BINDING_SSBO_PALETTE = 0;

        static constexpr uint32_t TARGET_PALETTE_SIZE = 0;
        static constexpr uint32_t TARGET_PALETTE_INTERVAL = 1;
        static constexpr uint32_t TARGET_PALETTE_OFFSET = 2;
        static constexpr uint32_t TARGET_PALETTE_SMOOTHING = 3;
        static constexpr uint32_t TARGET_PALETTE_SINGLE_SMOOTHING = 4;
        static constexpr uint32_t TARGET_PALETTE_ANIMATION_SPEED = 5;
        static constexpr uint32_t TARGET_PALETTE_COLORS = 6;

        void configure(vkh::Core &core, std::vector<vkh::DescriptorManager> &managers) override {
            auto bufferManager = vkh::HostDataObjectManager();
            bufferManager.reserve<uint32_t>(TARGET_PALETTE_SIZE);
            bufferManager.reserve<float>(TARGET_PALETTE_INTERVAL);
            bufferManager.reserve<double>(TARGET_PALETTE_OFFSET);
            bufferManager.reserve<uint32_t>(TARGET_PALETTE_SMOOTHING);
            bufferManager.reserve<uint32_t>(TARGET_PALETTE_SINGLE_SMOOTHING);
            bufferManager.reserve<float>(TARGET_PALETTE_ANIMATION_SPEED, 4);
            bufferManager.reserveArray<glm::vec4>(TARGET_PALETTE_COLORS, 0);

            auto ssbo = std::make_unique<vkh::ShaderStorage>(core, std::move(bufferManager),
                                                             vkh::BufferLocalization::BIDIRECTIONAL, false);
            auto descManager = vkh::DescriptorManager();

            descManager.appendSSBO(BINDING_SSBO_PALETTE, STAGE, std::move(ssbo));
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
        static constexpr uint32_t TARGET_STRIPE_ITERATION_COLORING = 6;


        void configure(vkh::Core &core, std::vector<vkh::DescriptorManager> &managers) override {
            auto bufferManager = vkh::HostDataObjectManager();
            bufferManager.reserve<uint32_t>(TARGET_STRIPE_TYPE);
            bufferManager.reserve<float>(TARGET_STRIPE_FIRST_INTERVAL);
            bufferManager.reserve<float>(TARGET_STRIPE_SECOND_INTERVAL);
            bufferManager.reserve<float>(TARGET_STRIPE_OPACITY);
            bufferManager.reserve<float>(TARGET_STRIPE_OFFSET);
            bufferManager.reserve<float>(TARGET_STRIPE_ANIMATION_SPEED);
            bufferManager.reserve<uint32_t>(TARGET_STRIPE_ITERATION_COLORING);
            auto ubo = std::make_unique<vkh::Uniform>(core, std::move(bufferManager),
                                                      vkh::BufferLocalization::BIDIRECTIONAL, false);
            auto descManager = vkh::DescriptorManager();
            descManager.appendUBO(BINDING_UBO_STRIPE, STAGE, std::move(ubo));
            managers.emplace_back(std::move(descManager));
        }
    };

    struct DescSlope final : public vkh::DescriptorTemplate {
        static constexpr uint32_t ID = 5;
        static constexpr VkShaderStageFlags STAGE = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;

        static constexpr uint32_t BINDING_UBO_SLOPE = 0;

        static constexpr uint32_t TARGET_SLOPE_DEPTH = 0;
        static constexpr uint32_t TARGET_SLOPE_REFLECTION_RATIO = 1;
        static constexpr uint32_t TARGET_SLOPE_OPACITY = 2;
        static constexpr uint32_t TARGET_SLOPE_ZENITH = 3;
        static constexpr uint32_t TARGET_SLOPE_AZIMUTH = 4;


        void configure(vkh::Core &core, std::vector<vkh::DescriptorManager> &managers) override {
            auto bufferManager = vkh::HostDataObjectManager();
            bufferManager.reserve<float>(TARGET_SLOPE_DEPTH);
            bufferManager.reserve<float>(TARGET_SLOPE_REFLECTION_RATIO);
            bufferManager.reserve<float>(TARGET_SLOPE_OPACITY);
            bufferManager.reserve<float>(TARGET_SLOPE_ZENITH);
            bufferManager.reserve<float>(TARGET_SLOPE_AZIMUTH);
            auto ubo = std::make_unique<vkh::Uniform>(core, std::move(bufferManager),
                                                      vkh::BufferLocalization::BIDIRECTIONAL, true);
            auto descManager = vkh::DescriptorManager();
            descManager.appendUBO(BINDING_UBO_SLOPE, STAGE, std::move(ubo));
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


        void configure(vkh::Core &core, std::vector<vkh::DescriptorManager> &managers) override {
            auto bufferManager = vkh::HostDataObjectManager();
            bufferManager.reserve<float>(TARGET_COLOR_GAMMA);
            bufferManager.reserve<float>(TARGET_COLOR_EXPOSURE);
            bufferManager.reserve<float>(TARGET_COLOR_HUE);
            bufferManager.reserve<float>(TARGET_COLOR_SATURATION);
            bufferManager.reserve<float>(TARGET_COLOR_BRIGHTNESS);
            bufferManager.reserve<float>(TARGET_COLOR_CONTRAST);
            auto ubo = std::make_unique<vkh::Uniform>(core, std::move(bufferManager),
                                                      vkh::BufferLocalization::BIDIRECTIONAL, false);
            auto descManager = vkh::DescriptorManager();
            descManager.appendUBO(BINDING_UBO_COLOR, STAGE, std::move(ubo));
            managers.emplace_back(std::move(descManager));
        }
    };

    struct DescFog final : public vkh::DescriptorTemplate {
        static constexpr uint32_t ID = 7;
        static constexpr VkShaderStageFlags STAGE = VK_SHADER_STAGE_FRAGMENT_BIT;

        static constexpr uint32_t BINDING_UBO_FOG = 0;

        static constexpr uint32_t TARGET_FOG_RADIUS = 0;
        static constexpr uint32_t TARGET_FOG_OPACITY = 1;

        void configure(vkh::Core &core, std::vector<vkh::DescriptorManager> &managers) override {
            auto descManager = vkh::DescriptorManager();

            auto bufferManager = vkh::HostDataObjectManager();
            bufferManager.reserve<float>(TARGET_FOG_RADIUS);
            bufferManager.reserve<float>(TARGET_FOG_OPACITY);
            auto ubo = std::make_unique<vkh::Uniform>(core, std::move(bufferManager),
                                                      vkh::BufferLocalization::BIDIRECTIONAL, false);
            descManager.appendUBO(BINDING_UBO_FOG, STAGE, std::move(ubo));
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


        void configure(vkh::Core &core, std::vector<vkh::DescriptorManager> &managers) override {
            auto bufferManager = vkh::HostDataObjectManager();
            bufferManager.reserve<float>(TARGET_BLOOM_THRESHOLD);
            bufferManager.reserve<float>(TARGET_BLOOM_RADIUS);
            bufferManager.reserve<float>(TARGET_BLOOM_SOFTNESS);
            bufferManager.reserve<float>(TARGET_BLOOM_INTENSITY);
            auto ubo = std::make_unique<vkh::Uniform>(core, std::move(bufferManager),
                                                      vkh::BufferLocalization::BIDIRECTIONAL, false);
            auto descManager = vkh::DescriptorManager();
            descManager.appendUBO(BINDING_UBO_BLOOM, STAGE, std::move(ubo));
            managers.emplace_back(std::move(descManager));
        }
    };

    struct DescNoiseReduction final : public vkh::DescriptorTemplate {
        static constexpr uint32_t ID = 9;
        static constexpr VkShaderStageFlags STAGE = VK_SHADER_STAGE_FRAGMENT_BIT;

        static constexpr uint32_t BINDING_UBO_NOISE_REDUCTION = 0;

        static constexpr uint32_t TARGET_NOISE_REDUCTION_USE = 0;
        static constexpr uint32_t TARGET_NOISE_REDUCTION_SIMILAR_COUNT_THRESHOLD = 1;
        static constexpr uint32_t TARGET_NOISE_REDUCTION_DIFFERENCE_THRESHOLD = 2;


        void configure(vkh::Core &core, std::vector<vkh::DescriptorManager> &managers) override {
            auto bufferManager = vkh::HostDataObjectManager();
            bufferManager.reserve<bool>(TARGET_NOISE_REDUCTION_USE, 3);
            bufferManager.reserve<uint32_t>(TARGET_NOISE_REDUCTION_SIMILAR_COUNT_THRESHOLD);
            bufferManager.reserve<float>(TARGET_NOISE_REDUCTION_DIFFERENCE_THRESHOLD);
            auto ubo = std::make_unique<vkh::Uniform>(core, std::move(bufferManager),
                                                      vkh::BufferLocalization::BIDIRECTIONAL, false);
            auto descManager = vkh::DescriptorManager();
            descManager.appendUBO(BINDING_UBO_NOISE_REDUCTION, STAGE, std::move(ubo));
            managers.emplace_back(std::move(descManager));
        }
    };

    struct DescVideo final : public vkh::DescriptorTemplate {
        static constexpr uint32_t ID = 10;
        static constexpr VkShaderStageFlags STAGE = VK_SHADER_STAGE_FRAGMENT_BIT | VK_SHADER_STAGE_COMPUTE_BIT;

        static constexpr uint32_t BINDING_UBO_VIDEO = 0;

        static constexpr uint32_t TARGET_VIDEO_DEFAULT_ZOOM_INCREMENT = 0;
        static constexpr uint32_t TARGET_VIDEO_CURRENT_FRAME = 1;


        void configure(vkh::Core &core, std::vector<vkh::DescriptorManager> &managers) override {
            auto bufferManager = vkh::HostDataObjectManager();
            bufferManager.reserve<float>(TARGET_VIDEO_DEFAULT_ZOOM_INCREMENT);
            bufferManager.reserve<float>(TARGET_VIDEO_CURRENT_FRAME);
            auto ubo = std::make_unique<vkh::Uniform>(core, std::move(bufferManager),
                                                      vkh::BufferLocalization::BIDIRECTIONAL, true);
            auto descManager = vkh::DescriptorManager();
            descManager.appendUBO(BINDING_UBO_VIDEO, STAGE, std::move(ubo));
            managers.emplace_back(std::move(descManager));
        }
    };

    struct DescFractal3D final : public vkh::DescriptorTemplate {
        static constexpr uint32_t ID = 11;
        static constexpr VkShaderStageFlags STAGE = VK_SHADER_STAGE_VERTEX_BIT;

        static constexpr uint32_t BINDING_UBO_F3D = 0;

        static constexpr uint32_t TARGET_F3D_BASE_ITERATION = 0;
        static constexpr uint32_t TARGET_F3D_DEPTH_DIVISOR = 1;
        static constexpr uint32_t TARGET_F3D_ROTATION = 2;


        void configure(vkh::Core &core, std::vector<vkh::DescriptorManager> &managers) override {
            auto bufferManager = vkh::HostDataObjectManager();
            bufferManager.reserve<float>(TARGET_F3D_BASE_ITERATION);
            bufferManager.reserve<float>(TARGET_F3D_DEPTH_DIVISOR);
            bufferManager.reserve<float>(TARGET_F3D_ROTATION);
            auto ubo = std::make_unique<vkh::Uniform>(core, std::move(bufferManager),
                                                      vkh::BufferLocalization::BIDIRECTIONAL, false);
            auto descManager = vkh::DescriptorManager();
            descManager.appendUBO(BINDING_UBO_F3D, STAGE, std::move(ubo));
            managers.emplace_back(std::move(descManager));
        }
    };

    struct DescRenderMeta final : public vkh::DescriptorTemplate {
        static constexpr uint32_t ID = 12;
        static constexpr VkShaderStageFlags STAGE = VK_SHADER_STAGE_COMPUTE_BIT;
        static constexpr uint32_t BINDING_RM_SSBO = 0;
        static constexpr uint32_t TARGET_RM_MAX_ITERATION = 0;
        static constexpr uint32_t TARGET_RM_MAX_REF_ITERATION = 1;
        static constexpr uint32_t TARGET_RM_LOG_ZOOM = 2;
        static constexpr uint32_t TARGET_RM_BAILOUT = 3;
        static constexpr uint32_t TARGET_RM_CLARITY_MULTIPLIER = 4;
        static constexpr uint32_t TARGET_RM_DECIMALIZE_ITERATION_METHOD = 5;
        static constexpr uint32_t TARGET_RM_OFFSET = 6;
        static constexpr uint32_t TARGET_RM_ORBIT = 7;


        static constexpr uint32_t BINDING_RM_TABLE_SSBO = 1;
        static constexpr uint32_t TARGET_RM_TABLE_LEN = 0;
        static constexpr uint32_t TARGET_RM_TABLE_SELECTION_METHOD = 1;
        static constexpr uint32_t TARGET_RM_TABLE_DATA = 2;

        static constexpr uint32_t BINDING_RM_MAPPER_SSBO = 2;
        static constexpr uint32_t TARGET_RM_MAPPER_LEN = 0;
        static constexpr uint32_t TARGET_RM_MAPPER_DATA = 1;

        static constexpr uint32_t BINDING_RM_BATCH_INFO_UBO = 3;
        static constexpr uint32_t TARGET_RM_BATCH_SIZE = 0;

        static constexpr uint32_t BINDING_RM_BATCH_SSBO = 4;
        static constexpr uint32_t TARGET_RM_BATCH_STAGING_DATA = 0;

        static constexpr uint32_t BINDING_RM_BATCH_RESULT_SSBO = 5;
        static constexpr uint32_t TARGET_RM_BATCH_RESULT_COMPLETED = 0;

        void configure(vkh::Core &core, std::vector<vkh::DescriptorManager> &managers) override {

            static_assert(sizeof(PA<float>) == 32);
            static_assert(alignof(PA<float>) == 8);
            static_assert(sizeof(ComputeShaderBatchStagingData) == 32);
            static_assert(alignof(ComputeShaderBatchStagingData) == 8);

            vkh::HostDataObjectManager homRm;
            homRm.reserve<uint64_t>(TARGET_RM_MAX_ITERATION);
            homRm.reserve<uint64_t>(TARGET_RM_MAX_REF_ITERATION);
            homRm.reserve<float>(TARGET_RM_LOG_ZOOM);
            homRm.reserve<float>(TARGET_RM_BAILOUT);
            homRm.reserve<float>(TARGET_RM_CLARITY_MULTIPLIER);
            homRm.reserve<uint32_t>(TARGET_RM_DECIMALIZE_ITERATION_METHOD);
            homRm.reserve<complex<float>>(TARGET_RM_OFFSET);
            homRm.reserveArray<complex<float>>(TARGET_RM_ORBIT, 0);


            vkh::HostDataObjectManager homRmTable;
            homRmTable.reserve<uint64_t>(TARGET_RM_TABLE_LEN);
            homRmTable.reserve<uint32_t>(TARGET_RM_TABLE_SELECTION_METHOD, 4);
            homRmTable.reserveArray<PA<float>>(TARGET_RM_TABLE_DATA, 0);

            vkh::HostDataObjectManager homRmMapper;
            homRmMapper.reserve<uint64_t>(TARGET_RM_MAPPER_LEN);
            homRmMapper.reserveArray<MPAIndexMapper>(TARGET_RM_MAPPER_DATA, 0);

            vkh::HostDataObjectManager homRmBatchInfo;
            homRmBatchInfo.reserve<uint32_t>(TARGET_RM_BATCH_SIZE);

            vkh::HostDataObjectManager homRmBatch;
            homRmBatch.reserveArray<ComputeShaderBatchStagingData>(TARGET_RM_BATCH_STAGING_DATA, 1);

            vkh::HostDataObjectManager homRmBatchResult;
            homRmBatchResult.reserveArray<uint32_t>(TARGET_RM_BATCH_RESULT_COMPLETED, 1);

            auto rmSSBO = std::make_unique<vkh::ShaderStorage>(core, std::move(homRm),
                                                               vkh::BufferLocalization::UNIDIRECTIONAL, false);
            auto rmTableSSBO = std::make_unique<vkh::ShaderStorage>(core, std::move(homRmTable),
                                                                    vkh::BufferLocalization::UNIDIRECTIONAL, false);
            auto rmMapperSSBO = std::make_unique<vkh::ShaderStorage>(core, std::move(homRmMapper),
                                                                     vkh::BufferLocalization::UNIDIRECTIONAL, false);

            auto rmBatchInfoUBO = std::make_unique<vkh::Uniform>(core, std::move(homRmBatchInfo),
                                                                 vkh::BufferLocalization::UNIDIRECTIONAL, false);
            auto rmBatchSSBO = std::make_unique<vkh::ShaderStorage>(core, std::move(homRmBatch),
                                                                    vkh::BufferLocalization::UNIDIRECTIONAL, false);
            auto rmBatchResultSSBO = std::make_unique<vkh::ShaderStorage>(
                    core, std::move(homRmBatchResult), vkh::BufferLocalization::BIDIRECTIONAL, false);


            vkh::DescriptorManager descManagerRenderMeta;
            descManagerRenderMeta.appendSSBO(BINDING_RM_SSBO, VK_SHADER_STAGE_COMPUTE_BIT, std::move(rmSSBO));
            descManagerRenderMeta.appendSSBO(BINDING_RM_TABLE_SSBO, VK_SHADER_STAGE_COMPUTE_BIT,
                                             std::move(rmTableSSBO));
            descManagerRenderMeta.appendSSBO(BINDING_RM_MAPPER_SSBO, VK_SHADER_STAGE_COMPUTE_BIT,
                                             std::move(rmMapperSSBO));
            descManagerRenderMeta.appendUBO(BINDING_RM_BATCH_INFO_UBO, VK_SHADER_STAGE_COMPUTE_BIT,
                                            std::move(rmBatchInfoUBO));
            descManagerRenderMeta.appendSSBO(BINDING_RM_BATCH_SSBO, VK_SHADER_STAGE_COMPUTE_BIT,
                                             std::move(rmBatchSSBO));
            descManagerRenderMeta.appendSSBO(BINDING_RM_BATCH_RESULT_SSBO, VK_SHADER_STAGE_COMPUTE_BIT,
                                             std::move(rmBatchResultSSBO));
            
            managers.emplace_back(std::move(descManagerRenderMeta));
        }
    };

    struct DescRenderMetaIterationVariant : public vkh::DescriptorTemplate {

        static constexpr uint32_t ID = 13;
        static constexpr VkShaderStageFlags STAGE = VK_SHADER_STAGE_COMPUTE_BIT;

        void configure(vkh::Core &core, std::vector<vkh::DescriptorManager> &managers) override {
            DescIteration().configure(core, managers);
        };
    };
} // namespace merutilm::rff2::SharedDescriptorTemplate
