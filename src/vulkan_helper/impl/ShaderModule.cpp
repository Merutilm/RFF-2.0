//
// Created by Merutilm on 2025-07-13.
//

#include "ShaderModule.hpp"

#include <algorithm>

#include "../exception/exception.hpp"
#include <fstream>

namespace merutilm::vkh {
    ShaderModule::ShaderModule(const Core &core, const std::string &filename) : CoreHandler(core), shaderStage(getShaderStage(filename)),
                                                         filename(SHADER_PATH_PREFIX + filename + ".spv") {
        ShaderModule::init();
    }

    ShaderModule::~ShaderModule() {
        ShaderModule::destroy();
    }

    VkShaderStageFlagBits ShaderModule::getShaderStage(const std::string &filename) {
        std::string filenameLower = filename;
        std::ranges::transform(filename, filenameLower.begin(), tolower);

        if (filenameLower.ends_with(".vert")) return VK_SHADER_STAGE_VERTEX_BIT;
        if (filenameLower.ends_with(".frag")) return VK_SHADER_STAGE_FRAGMENT_BIT;
        if (filenameLower.ends_with(".geom")) return VK_SHADER_STAGE_GEOMETRY_BIT;
        if (filenameLower.ends_with(".comp")) return VK_SHADER_STAGE_COMPUTE_BIT;
        throw exception_invalid_args("unsupported extension : " + filenameLower.substr(filenameLower.find_last_of('.') + 1));
    }



    void ShaderModule::init() {
        std::ifstream file(filename, std::ios::binary);
        if (!file.is_open()) {
            throw exception_invalid_args("invalid filename : " + filename);
        }
        auto code = std::vector(std::istreambuf_iterator(file), {});
        const VkShaderModuleCreateInfo info = {
            .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .codeSize = code.size(),
            .pCode = reinterpret_cast<uint32_t *>(code.data()),
        };
        if (vkCreateShaderModule(core.getLogicalDevice().getLogicalDeviceHandle(), &info, nullptr, &shaderModule) != VK_SUCCESS) {
            throw exception_init("Failed to create shader module!");
        }
    }

    void ShaderModule::destroy() {
        vkDestroyShaderModule(core.getLogicalDevice().getLogicalDeviceHandle(), shaderModule, nullptr);
    }
}
