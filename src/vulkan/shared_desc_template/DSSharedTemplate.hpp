//
// Created by Merutilm on 2025-07-19.
//

#pragma once
#include <memory>
#include <glm/glm.hpp>

#include "../../vulkan_helper/manage/DescriptorManager.hpp"
#include "../../vulkan_helper/struct/DescriptorTemplate.hpp"

namespace merutilm::rff2::DSSharedTemplate {

    struct Camera3D final : public mvk::DescriptorTemplate{

        static constexpr uint32_t ID = 0;
        static constexpr uint32_t UBO_CAMERA = 0;
        static constexpr uint32_t CAMERA_MODEL = 0;
        static constexpr uint32_t CAMERA_VIEW = 1;
        static constexpr uint32_t CAMERA_PROJ = 2;

        std::unique_ptr<mvk::DescriptorManager> create(const mvk::Core &core, const VkShaderStageFlags useStage) override {
            auto bufferManager = std::make_unique<mvk::ShaderObjectManager>();

            bufferManager->reserve<glm::mat4>(CAMERA_MODEL);
            bufferManager->reserve<glm::mat4>(CAMERA_VIEW);
            bufferManager->reserve<glm::mat4>(CAMERA_PROJ);

            auto ubo = std::make_unique<mvk::Uniform>(core, std::move(bufferManager));
            auto descManager = std::make_unique<mvk::DescriptorManager>();
            descManager->appendUBO(UBO_CAMERA, useStage, std::move(ubo));
            return descManager;
        }
    };
    struct Time final : public mvk::DescriptorTemplate{

        static constexpr uint32_t ID = 1;
        static constexpr uint32_t UBO_TIME = 0;
        static constexpr uint32_t TIME_CURRENT = 0;

        std::unique_ptr<mvk::DescriptorManager> create(const mvk::Core &core, const VkShaderStageFlags useStage) override {
            auto bufferManager = std::make_unique<mvk::ShaderObjectManager>();
            bufferManager->reserve<float>(TIME_CURRENT);
            auto ubo = std::make_unique<mvk::Uniform>(core, std::move(bufferManager));
            auto descManager = std::make_unique<mvk::DescriptorManager>();
            descManager->appendUBO(UBO_TIME, useStage, std::move(ubo));
            return descManager;
        }
    };

}
