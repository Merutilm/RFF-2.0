//
// Created by Merutilm on 2025-07-13.
//

#pragma once
#include <vulkan_helper/handle/CoreHandler.hpp>

namespace merutilm::vkh {
    class ShaderModule final : public CoreHandler {

        static constexpr auto SHADER_PATH_PREFIX = "shaders/";

        VkShaderStageFlagBits shaderStage;
        VkShaderModule shaderModule = nullptr;
        const std::vector<char> code = {};
        const std::string filename;

    public:
        explicit ShaderModule(Core & core, const std::string &filename);

        ~ShaderModule() override;

        ShaderModule(const ShaderModule &) = delete;

        ShaderModule &operator=(const ShaderModule &) = delete;

        ShaderModule(ShaderModule &&) = delete;

        ShaderModule &operator=(ShaderModule &&) = delete;

        [[nodiscard]] VkShaderStageFlagBits getShaderStage() const { return shaderStage; }

        [[nodiscard]] VkShaderModule getShaderModuleHandle() const { return shaderModule; }

        [[nodiscard]] const std::vector<char> &getCode() const { return code; }

    protected:

        void init() override;

        void cleanup() override;

    private:
        [[nodiscard]] static VkShaderStageFlagBits findShaderStageForName(const std::string &filename);
    };

    

}
