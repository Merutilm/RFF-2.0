//
// Created by Merutilm on 2026-02-03.
//
#include <vulkan_helper/engine/repo/GlobalSamplerRepo.hpp>

namespace merutilm::vkh {
    Sampler & GlobalSamplerRepo::pick(const VkSamplerCreateInfo &samplerCreateInfo) {
        auto it = repository.find(samplerCreateInfo);
        if (it == repository.end()) {
            auto[newIt, _] = repository.try_emplace(samplerCreateInfo, std::make_unique<Sampler>(core, samplerCreateInfo));
            it = newIt;
        }
        return *it->second;
    }
}
