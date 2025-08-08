//
// Created by Merutilm on 2025-07-19.
//

#pragma once
#include <memory>
#include <string>
#include <glm/glm.hpp>

#include "../manage/DescriptorManager.hpp"

namespace merutilm::mvk::DSSharedTemplate {

    struct Camera3D {
        static constexpr uint32_t UBO_CAMERA = 0;
        static constexpr uint32_t CAMERA_MODEL = 0;
        static constexpr uint32_t CAMERA_VIEW = 1;
        static constexpr uint32_t CAMERA_PROJ = 2;

        static std::unique_ptr<DescriptorManager> create(const Core &core, const VkShaderStageFlags useStage) {
            auto bufferManager = std::make_unique<ShaderObjectManager>();

            bufferManager->reserve<glm::mat4>(CAMERA_MODEL);
            bufferManager->reserve<glm::mat4>(CAMERA_VIEW);
            bufferManager->reserve<glm::mat4>(CAMERA_PROJ);

            auto ubo = std::make_unique<Uniform>(core, std::move(bufferManager));
            auto descManager = std::make_unique<DescriptorManager>();
            descManager->appendUBO(UBO_CAMERA, useStage, std::move(ubo));
            return descManager;
        }
    };
    struct Time {
        static constexpr uint32_t UBO_TIME = 0;
        static constexpr uint32_t TIME_CURRENT = 0;

        static std::unique_ptr<DescriptorManager> create(const Core &core, const VkShaderStageFlags useStage) {
            auto bufferManager = std::make_unique<ShaderObjectManager>();
            bufferManager->reserve<float>(TIME_CURRENT);
            auto ubo = std::make_unique<Uniform>(core, std::move(bufferManager));
            auto descManager = std::make_unique<DescriptorManager>();
            descManager->appendUBO(UBO_TIME, useStage, std::move(ubo));
            return descManager;
        }
    };

}
