//
// Created by Merutilm on 2025-07-13.
//

#include "ShaderModule.hpp"

#include <algorithm>

#include "../core/vkh_core.hpp"
#include <fstream>

namespace merutilm::vkh {
    ShaderModuleImpl::ShaderModuleImpl(CoreRef core, const std::string &filename) : CoreHandler(core), shaderStage(getShaderStage(filename)),
                                                         filename(SHADER_PATH_PREFIX + filename + ".spv") {
        ShaderModuleImpl::init();
    }

    ShaderModuleImpl::~ShaderModuleImpl() {
        ShaderModuleImpl::destroy();
    }

    VkShaderStageFlagBits ShaderModuleImpl::getShaderStage(const std::string &filename) {
        std::string filenameLower = filename;
        std::ranges::transform(filename, filenameLower.begin(), tolower);

        if (filenameLower.ends_with(".vert")) return VK_SHADER_STAGE_VERTEX_BIT;
        if (filenameLower.ends_with(".frag")) return VK_SHADER_STAGE_FRAGMENT_BIT;
        if (filenameLower.ends_with(".geom")) return VK_SHADER_STAGE_GEOMETRY_BIT;
        if (filenameLower.ends_with(".comp")) return VK_SHADER_STAGE_COMPUTE_BIT;
        throw exception_invalid_args("unsupported extension : " + filenameLower.substr(filenameLower.find_last_of('.') + 1));
    }



    void ShaderModuleImpl::init() {
        std::array<wchar_t, MAX_PATH> modulePath;
        GetModuleFileNameW(nullptr, modulePath.data(), modulePath.size());
        std::ifstream file(std::filesystem::path(modulePath.data()) / ".." / filename, std::ios::binary);
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
        if (allocator::invoke(vkCreateShaderModule, core.getLogicalDevice().getLogicalDeviceHandle(), &info, nullptr, &shaderModule) != VK_SUCCESS) {
            throw exception_init("Failed to create shader module!");
        }
    }

    void ShaderModuleImpl::destroy() {
        allocator::invoke(vkDestroyShaderModule, core.getLogicalDevice().getLogicalDeviceHandle(), shaderModule, nullptr);
    }
}
