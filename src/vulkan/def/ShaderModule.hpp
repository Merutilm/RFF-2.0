//
// Created by Merutilm on 2025-07-13.
//

#pragma once
#include <string>
#include <vector>
#include <vulkan/vulkan_core.h>

#include "../handle/CoreHandler.hpp"

namespace merutilm::mvk {
    class ShaderModule final : public CoreHandler {
        VkShaderStageFlagBits shaderStage;
        VkShaderModule shaderModule = nullptr;
        const std::vector<char> code = {};
        const std::string filename;

    public:
        explicit ShaderModule(const Core &core, const std::string &filename);

        ~ShaderModule() override;

        ShaderModule(const ShaderModule &) = delete;

        ShaderModule &operator=(const ShaderModule &) = delete;

        ShaderModule(ShaderModule &&) = delete;

        ShaderModule &operator=(ShaderModule &&) = delete;

        [[nodiscard]] VkShaderStageFlagBits getShaderStage() const { return shaderStage; }

        [[nodiscard]] VkShaderModule getShaderModuleHandle() const { return shaderModule; }

        [[nodiscard]] const std::vector<char> &getCode() const { return code; }

        [[nodiscard]] static VkShaderStageFlagBits getShaderStage(const std::string &filename);

    private:
        void init() override;

        void destroy() override;
    };
}
