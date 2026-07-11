//
// Created by Merutilm on 2026-02-03.
//

#include <vulkan_helper/base/vkh_base.hpp>
#include <vulkan_helper/engine/repo/GlobalShaderModuleRepo.hpp>

namespace merutilm::vkh {
    ShaderModule & GlobalShaderModuleRepo::pick(const std::string &filename) {
        auto it = repository.find(filename);
        if (it == repository.end()) {
            auto [newIt, _] = repository.try_emplace(filename, std::make_unique<ShaderModule>(core, filename));
            it = newIt;
        }
        return *it->second;
    }
}
